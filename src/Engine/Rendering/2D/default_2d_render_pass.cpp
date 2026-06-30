#include "Engine/Rendering/2D/default_2d_render_pass.h"

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Layers/graphics_pipeline_manager.h"
#include "Engine/Layers/texture_manager.h"
#include "Engine/Layers/texture_sampler_manager.h"
#include "Engine/Layers/material_manager.h"

namespace Engine {
  Default2DRenderPass::Default2DRenderPass(StorageBuffer &spriteData,
                                           StorageBuffer &entityIndexData,
                                           Uint32 renderTargetWidth,
                                           Uint32 renderTargetHeight) : RenderPass(RenderPassCreateInfo{
    .renderTargetWidth = renderTargetWidth, .renderTargetHeight = renderTargetHeight
  }) {
    m_spriteDataBuffer = &spriteData;
    m_entityIndexBuffer = &entityIndexData;
    m_frameData = nullptr;
  }

  void Default2DRenderPass::PrepareData(FrameRenderData &frameRenderData) {
    m_frameData = &frameRenderData;
  }

  bool Default2DRenderPass::Render(SDL_GPUCommandBuffer *commandBuffer) {
    if (!m_frameData || m_frameData->views.empty()) return true;

    // Prepare for render pass — clear once, all views draw into the same target
    std::array targetInfos{
      SDL_GPUColorTargetInfo{
        .texture = GetRenderTarget()->texture,
        .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .cycle = false,
      }
    };
    auto renderPass{SDL_BeginGPURenderPass(commandBuffer, targetInfos.data(), targetInfos.size(), nullptr)};
    if (!renderPass) {
      ENGINE_LOG_SDL_ERROR("Unable to begin render pass.");
      return false;
    }

    // Bind vertex storage buffers once — shared across all views
    std::array vertexStorageBuffers{m_spriteDataBuffer->buffer, m_entityIndexBuffer->buffer};
    SDL_BindGPUVertexStorageBuffers(renderPass, 0, vertexStorageBuffers.data(), vertexStorageBuffers.size());

    // Iterate over each view (camera), setting its viewport/scissor and drawing its batches
    int boundTextureId{-1};
    for (const auto &view: m_frameData->views) {
      // Set viewport and scissor for this view's region of the render target
      SDL_SetGPUViewport(renderPass, &view.viewport);
      SDL_SetGPUScissor(renderPass, &view.scissor);

      // Draw this view's pipeline state groups
      int startIndex = view.baseVertexIndex;
      for (const auto &pipelineData: view.stateData.data) {
        auto *material = App::GetLayer<MaterialManager>().GetMaterial(pipelineData.materialId);
        if (!material) {
          ENGINE_LOG_ERROR(std::format("Unable to get material with id {}", pipelineData.materialId));
          return false;
        }

        auto pipeline = App::GetLayer<GraphicsPipelineManager>().GetPipeline(material->shaderId);
        if (!pipeline) {
          ENGINE_LOG_ERROR(std::format("Unable to get pipeline with id {}", material->shaderId));
          return false;
        }
        SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

        /// Texture bindings are costly, so do it only when needed
        if (boundTextureId != material->textureId) {
          boundTextureId = material->textureId;
          const auto textureData = App::GetLayer<TextureManager>().GetTexture(
            static_cast<Uint16>(boundTextureId));
          if (!textureData) {
            ENGINE_LOG_SDL_ERROR("Invalid texture data. Make sure the texture is properly registered before using it.");
            return false;
          }
          std::array textureSamplerBindings{
            SDL_GPUTextureSamplerBinding{
              .texture = textureData->texture,
              .sampler = App::GetLayer<TextureSamplerManager>().GetSampler(LINEAR_CLAMP),
            },
          };
          SDL_BindGPUFragmentSamplers(renderPass, 0, textureSamplerBindings.data(), textureSamplerBindings.size());
        }

        if (material->uniformBuffer.Valid()) {
          std::array fragmentStorageBuffers{material->uniformBuffer.buffer};
          SDL_BindGPUFragmentStorageBuffers(renderPass, 0, fragmentStorageBuffers.data(),
                                            fragmentStorageBuffers.size());
        }

        // View matrix from this view's render pass state data
        auto cameraView = view.stateData.viewMatrix;
        SDL_PushGPUVertexUniformData(commandBuffer, 0, &cameraView, sizeof(glm::mat4));

        auto vertCount = static_cast<int>(pipelineData.itemCount) * 6;
        SDL_DrawGPUPrimitives(
          renderPass,
          vertCount,
          1,
          startIndex,
          0
        );

        // Move to next index section
        startIndex += vertCount;
      }
    }

    SDL_EndGPURenderPass(renderPass);
    return true;
  }
}
