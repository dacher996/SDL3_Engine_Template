#include "Engine/Rendering/RenderPasses/render_target_render_pass.h"

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Core/logger.h"
#include "Engine/Layers/graphics_pipeline_manager.h"
#include "Engine/Layers/texture_sampler_manager.h"

namespace Engine {
  TextureToScreenRenderPass::TextureToScreenRenderPass(SDL_GPUTexture *swapchainTexture,
                                                       Texture *renderTarget,
                                                       LogicalPresentation mode)
    : m_swapchainTexture(swapchainTexture),
      m_presentationMode(mode),
      RenderPass(RenderPassCreateInfo{.renderTarget = renderTarget}) {
    // Constructor initializes the parent RenderPass class with the source render
    // target. The m_swapchainTexture is saved as the destination swapchain buffer
    // where we render.
  }

  std::array<float, 4> TextureToScreenRenderPass::CalculateScreenRect(LogicalPresentation mode) const {
    int w = 0, h = 0;
    SDL_GetWindowSizeInPixels(App::GetLayer<AppContext>().window, &w, &h);

    if (w <= 0 || h <= 0) {
      return {0.0f, 0.0f, 1.0f, 1.0f};
    }

    Vec2f windowSize(w, h);
    Vec2f targetSize(GetRenderTarget()->width, GetRenderTarget()->height);

    return GetNormalizedWindowRectSize(mode, windowSize, targetSize);
  }

  bool TextureToScreenRenderPass::Render(SDL_GPUCommandBuffer *cmdBuffer) {
    if (!m_swapchainTexture)
      return false;

    // Define the color target info for our destination (the swapchain texture).
    // Load operation is CLEAR to clear the screen before drawing (using solid
    // black), and store operation is STORE to keep the rendered quad.
    SDL_GPUColorTargetInfo swapchainTargetInfo = {
      .texture = m_swapchainTexture,
      .clear_color = (SDL_FColor){1.0f, 0.0f, 0.0f, 1.0f},
      .load_op = SDL_GPU_LOADOP_CLEAR,
      .store_op = SDL_GPU_STOREOP_STORE
    };

    // Begin the render pass with the swapchain texture.
    SDL_GPURenderPass *renderPass =
        SDL_BeginGPURenderPass(cmdBuffer, &swapchainTargetInfo, 1, nullptr);
    if (!renderPass) {
      ENGINE_LOG_SDL_ERROR("Unable to begin render pass for the swapchain.");
      return false;
    }

    // Retrieve pipeline 1 (the texture-to-screen pipeline) from the graphics
    // pipeline manager.
    auto pipeline =
        App::GetLayer<GraphicsPipelineManager>().GetPipeline(1);
    if (!pipeline) {
      ENGINE_LOG_ERROR("Unable to get pipeline with id: 1");
      SDL_EndGPURenderPass(renderPass);
      return false;
    }

    // Bind the shader graphics pipeline.
    SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

    // Calculate the aspect-ratio-corrected screen rectangle coordinates.
    std::array<float, 4> screenRect = CalculateScreenRect(m_presentationMode);

    // Push the calculated rectangle to the vertex shader uniform block at slot 0
    // (register b0, space1). The shader will use this to scale/translate the
    // generated quad to the computed rect.
    SDL_PushGPUVertexUniformData(cmdBuffer, 0, screenRect.data(),
                                 sizeof(screenRect));

    // Bind the 2D texture (the render target) and the linear clamp sampler to
    // slot 0 (register t0, space2). This allows the fragment shader to read and
    // sample the render target contents.
    SDL_GPUTextureSamplerBinding samplerBinding = {
      .texture = GetRenderTarget()->texture,
      .sampler = App::GetLayer<TextureSamplerManager>().GetSampler(
        POINT_CLAMP),
    };
    SDL_BindGPUFragmentSamplers(renderPass, 0, &samplerBinding, 1);

    // Draw the 6 vertices forming the 2 triangles of the target quad.
    // The vertex shader generates coordinates on the fly using SV_VertexID.
    SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);

    // Conclude the render pass.
    SDL_EndGPURenderPass(renderPass);

    return true;
  }
}
