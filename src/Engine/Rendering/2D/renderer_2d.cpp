#include "Engine/Rendering/2D/renderer_2d.h"

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Layers/material_manager.h"
#include "Engine/Rendering/2D/shape_2d_batch.h"
#include "Engine/Rendering/render_target_render_pass.h"

namespace Engine {
  static constexpr Uint32 STORAGE_BUFFER_DEFAULT_SIZE = 1000;

  Renderer2D::Renderer2D() {
    auto gpuDevice = App::GetLayer<AppContext>().gpuDevice;

    // Prepare buffers for storing sprite data
    m_spriteDataBuffer.Resize<GPUSprite>(gpuDevice, STORAGE_BUFFER_DEFAULT_SIZE);
    m_entityIndexBuffer.Resize<Uint32>(gpuDevice, STORAGE_BUFFER_DEFAULT_SIZE);

    // Set up the default pipeline
    UseDefaultPipeline();
  }

  Renderer2D::~Renderer2D() {
    auto gpuDevice = App::GetLayer<AppContext>().gpuDevice;
    m_spriteDataBuffer.Free(gpuDevice);
    m_entityIndexBuffer.Free(gpuDevice);

    if (m_usingDefaultPipeline && m_defaultPass) {
      delete m_defaultPass;
      m_defaultPass = nullptr;
    }
  }

  void Renderer2D::Submit(const SpriteSubmission &submission) {
    m_pendingSubmissions.push_back(submission);
  }

  void Renderer2D::Submit(const std::vector<SpriteSubmission> &submissions) {
    m_pendingSubmissions.insert(m_pendingSubmissions.end(), submissions.begin(),
                                submissions.end());
  }

  void Renderer2D::Submit(Shape2DBatch &shapeBatch) { Submit(shapeBatch.End()); }

  void Renderer2D::Present() {
    // Don't render anything if the setup is incomplete
    if (!m_spriteDataBuffer.Valid() || !m_entityIndexBuffer.Valid())
      return;
    if (m_renderPasses.empty())
      return;

    auto appContext = App::GetLayer<AppContext>();

    auto commandBuffer{SDL_AcquireGPUCommandBuffer(appContext.gpuDevice)};
    if (!commandBuffer) {
      ENGINE_LOG_SDL_ERROR("Unable to acquire GPU command buffer.");
      return;
    }

    SDL_GPUTexture *swapchainTexture{nullptr};
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, appContext.window,
                                               &swapchainTexture, nullptr,
                                               nullptr)) {
      ENGINE_LOG_SDL_ERROR("Unable to acquire swapchain texture.");
      return;
    }

    // If the swapchain texture is valid, upload data and execute passes
    if (swapchainTexture) {
      // Prepare the frame data from all pending submissions
      m_currentFrameData = PrepareFrame(m_pendingSubmissions);
      m_pendingSubmissions.clear();

      // Upload transfer buffers to GPU
      UploadBuffers(commandBuffer);
      // Submit entity material uniform data
      SubmitMaterialUniforms(commandBuffer);

      // Prepare and execute all content render passes
      for (auto *pass: m_renderPasses) {
        pass->PrepareData(m_currentFrameData);
        pass->Render(commandBuffer);
      }

      // Present: blit the last pass's render target to the swapchain
      auto *lastRenderTarget = m_renderPasses.back()->GetRenderTarget();
      TextureToScreenRenderPass presentPass(swapchainTexture, lastRenderTarget,
                                            appContext.logicalPresentation);
      presentPass.Render(commandBuffer);
    }

    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  void Renderer2D::SetRenderPipeline(std::vector<RenderPass *> passes) {
    // Clean up default pass if we were using it
    if (m_usingDefaultPipeline && m_defaultPass) {
      delete m_defaultPass;
      m_defaultPass = nullptr;
    }
    m_usingDefaultPipeline = false;
    m_renderPasses = std::move(passes);
  }

  void Renderer2D::UseDefaultPipeline() {
    // Clean up existing default pass if it exists
    if (m_defaultPass) {
      delete m_defaultPass;
      m_defaultPass = nullptr;
    }
    auto &appCtx = App::GetLayer<AppContext>();

    m_defaultPass =
        new Default2DRenderPass(m_spriteDataBuffer, m_entityIndexBuffer,
                                appCtx.logicalWidth, appCtx.logicalHeight);
    m_renderPasses = {m_defaultPass};
    m_usingDefaultPipeline = true;
  }

  StorageBuffer &Renderer2D::GetSpriteDataBuffer() { return m_spriteDataBuffer; }

  StorageBuffer &Renderer2D::GetEntityIndexBuffer() {
    return m_entityIndexBuffer;
  }

  FrameRenderData
  Renderer2D::PrepareFrame(const std::vector<SpriteSubmission> &submissions) {
    struct RenderCommand {
      Uint64 sortIndex;
      Uint16 materialId;
      Uint16 entityIndex; // index into this view's unique sprite data (before
      // global offset)
    };

    // Per-view intermediate data computed in the first pass
    struct ViewPrepData {
      std::vector<RenderCommand> renderQueue;
      std::vector<size_t> uniqueDataEntryIndices; // indices into the submission's
      // entries[] for unique sprites
      RenderPassStateData stateData;
    };

    std::vector<ViewPrepData> viewPreps(submissions.size());

    // Phase 1: Per-view sorting and pipeline grouping
    size_t totalUniqueSprites = 0;
    size_t totalEntityIndices = 0;
    for (size_t v = 0; v < submissions.size(); ++v) {
      const auto &entries = submissions[v].entries;
      auto &prep = viewPreps[v];

      // Assign data indices — consecutive entries with identical sprite data
      // share the same index (handles multi-shader entities that the scene
      // expanded into separate SpriteEntry items)
      std::vector<Uint16> dataIndices(entries.size());
      prep.uniqueDataEntryIndices.reserve(entries.size());

      if (!entries.empty()) {
        dataIndices[0] = 0;
        prep.uniqueDataEntryIndices.push_back(0);

        for (size_t i = 1; i < entries.size(); ++i) {
          const auto &prev = entries[i - 1].data;
          const auto &curr = entries[i].data;
          if (curr.x == prev.x && curr.y == prev.y &&
              curr.rotation == prev.rotation && curr.tex_x == prev.tex_x &&
              curr.tex_y == prev.tex_y) {
            dataIndices[i] = dataIndices[i - 1];
          } else {
            dataIndices[i] =
                static_cast<Uint16>(prep.uniqueDataEntryIndices.size());
            prep.uniqueDataEntryIndices.push_back(i);
          }
        }
      }

      // Build render commands with sort keys
      prep.renderQueue.reserve(entries.size());
      for (size_t i = 0; i < entries.size(); ++i) {
        Uint64 depth = static_cast<Uint64>(entries[i].depth) << 32;
        Uint64 material = static_cast<Uint64>(entries[i].materialId);
        prep.renderQueue.push_back({
          .sortIndex = depth | material,
          .materialId = entries[i].materialId,
          .entityIndex = dataIndices[i],
        });
      }

      // Sort by depth (layer-first), then pipeline (shader-second)
      std::ranges::sort(prep.renderQueue,
                        [](const RenderCommand &a, const RenderCommand &b) {
                          return a.sortIndex < b.sortIndex;
                        });

      // Build pipeline state groups
      prep.stateData.totalCount = prep.renderQueue.size();
      prep.stateData.viewMatrix = submissions[v].viewMatrix;
      prep.stateData.entityIndexes.reserve(prep.renderQueue.size());

      {
        Uint64 lastSortIndex = 0;
        Uint16 lastMaterialId = App::GetLayer<MaterialManager>().GetDefaultMaterial()->id;
        int currentCount = 0;
        if (!prep.renderQueue.empty()) {
          lastSortIndex = prep.renderQueue[0].sortIndex;
          lastMaterialId = prep.renderQueue[0].materialId;
        }

        for (const auto &command: prep.renderQueue) {
          prep.stateData.entityIndexes.push_back(command.entityIndex);
          if (auto currentSortIndex = command.sortIndex;
            currentSortIndex == lastSortIndex) {
            currentCount++;
          } else {
            prep.stateData.data.emplace_back(currentCount, lastMaterialId);
            currentCount = 1;
            lastSortIndex = currentSortIndex;
            lastMaterialId = command.materialId;
          }
        }

        if (currentCount > 0) {
          prep.stateData.data.emplace_back(currentCount, lastMaterialId);
        }
      }

      totalUniqueSprites += prep.uniqueDataEntryIndices.size();
      totalEntityIndices += prep.stateData.entityIndexes.size();
    }

    auto &appContext = App::GetLayer<AppContext>();
    auto gpuDevice = appContext.gpuDevice;

    // Phase 2: Merge all sprite data into the shared buffer
    {
      if (m_spriteDataBuffer.itemCount < totalUniqueSprites) {
        m_spriteDataBuffer.Resize<GPUSprite>(
          gpuDevice,
          static_cast<size_t>(static_cast<float>(totalUniqueSprites) * 1.2f));
      }

      auto dataPtr{
        static_cast<GPUSprite *>(SDL_MapGPUTransferBuffer(
          gpuDevice, m_spriteDataBuffer.transferBuffer, true))
      };
      if (!dataPtr) {
        ENGINE_LOG_SDL_ERROR("Unable to map sprite transfer buffer pointer.");
        return {};
      }

      size_t writeOffset = 0;
      for (size_t v = 0; v < submissions.size(); ++v) {
        const auto &entries = submissions[v].entries;
        for (size_t i = 0; i < viewPreps[v].uniqueDataEntryIndices.size(); ++i) {
          dataPtr[writeOffset + i] =
              entries[viewPreps[v].uniqueDataEntryIndices[i]].data;
        }
        writeOffset += viewPreps[v].uniqueDataEntryIndices.size();
      }

      SDL_UnmapGPUTransferBuffer(gpuDevice, m_spriteDataBuffer.transferBuffer);
    }

    // Phase 3: Merge all entity indices into the shared buffer (with offsets)
    {
      if (m_entityIndexBuffer.itemCount < totalEntityIndices) {
        m_entityIndexBuffer.Resize<Uint32>(
          gpuDevice,
          static_cast<size_t>(static_cast<float>(totalEntityIndices) * 1.2f));
      }

      auto dataPtr{
        static_cast<Uint32 *>(SDL_MapGPUTransferBuffer(
          gpuDevice, m_entityIndexBuffer.transferBuffer, true))
      };
      if (!dataPtr) {
        ENGINE_LOG_SDL_ERROR("Unable to map index transfer buffer pointer.");
        return {};
      }

      size_t writeOffset = 0;
      Uint32 spriteDataOffset = 0;

      for (size_t v = 0; v < submissions.size(); ++v) {
        const auto &entityIndexes = viewPreps[v].stateData.entityIndexes;
        for (size_t i = 0; i < entityIndexes.size(); ++i) {
          // Offset entity index values so they reference the correct position
          // in the merged sprite data buffer
          dataPtr[writeOffset + i] = entityIndexes[i] + spriteDataOffset;
        }
        writeOffset += entityIndexes.size();
        spriteDataOffset +=
            static_cast<Uint32>(viewPreps[v].uniqueDataEntryIndices.size());
      }

      SDL_UnmapGPUTransferBuffer(gpuDevice, m_entityIndexBuffer.transferBuffer);
    }

    // Phase 4: Build FrameRenderData with per-view state
    FrameRenderData frameData;
    frameData.views.reserve(submissions.size());

    int cumulativeVertexIndex = 0;
    for (size_t v = 0; v < submissions.size(); ++v) {
      ViewRenderData viewData;
      viewData.stateData = std::move(viewPreps[v].stateData);
      viewData.baseVertexIndex = cumulativeVertexIndex;

      // Viewport: use submission's custom viewport, or default to full render
      // target
      if (submissions[v].viewport.w > 0 && submissions[v].viewport.h > 0) {
        viewData.viewport = submissions[v].viewport;
      } else {
        viewData.viewport = {
          .x = 0,
          .y = 0,
          .w = static_cast<float>(appContext.logicalWidth),
          .h = static_cast<float>(appContext.logicalHeight),
          .min_depth = 0.0f,
          .max_depth = 1.0f,
        };
      }

      // Scissor: use submission's custom scissor, or default to full render
      // target
      if (submissions[v].scissor.w > 0 && submissions[v].scissor.h > 0) {
        viewData.scissor = submissions[v].scissor;
      } else {
        viewData.scissor = {
          .x = 0,
          .y = 0,
          .w = appContext.logicalWidth,
          .h = appContext.logicalHeight,
        };
      }

      cumulativeVertexIndex +=
          static_cast<int>(viewData.stateData.totalCount) * 6;
      frameData.views.push_back(std::move(viewData));
    }

    return frameData;
  }

  void Renderer2D::UploadBuffers(SDL_GPUCommandBuffer *commandBuffer) const {
    auto copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass) {
      ENGINE_LOG_SDL_ERROR("Unable to begin copy pass.");
      return;
    }

    // Upload sprite data to GPU buffer
    {
      SDL_GPUTransferBufferLocation location{
        .transfer_buffer = m_spriteDataBuffer.transferBuffer,
        .offset = 0,
      };
      SDL_GPUBufferRegion region{
        .buffer = m_spriteDataBuffer.buffer,
        .offset = 0,
        .size = m_spriteDataBuffer.size,
      };

      SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
    }

    // Upload entity index data to GPU buffer
    {
      SDL_GPUTransferBufferLocation location{
        .transfer_buffer = m_entityIndexBuffer.transferBuffer,
        .offset = 0,
      };
      SDL_GPUBufferRegion region{
        .buffer = m_entityIndexBuffer.buffer,
        .offset = 0,
        .size = m_entityIndexBuffer.size,
      };

      SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
    }

    SDL_EndGPUCopyPass(copyPass);
  }

  void Renderer2D::SubmitMaterialUniforms(SDL_GPUCommandBuffer *commandBuffer) const {
    auto &materialManager = App::GetLayer<MaterialManager>();
    auto &materials = materialManager.GetMaterials();

    bool anyDirty = false;
    for (const auto &[id, material]: materials) {
      if (material->needsUpload && material->uniformBuffer.Valid()) {
        anyDirty = true;
        break;
      }
    }

    if (!anyDirty) return;

    auto copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass) {
      ENGINE_LOG_SDL_ERROR("Unable to begin copy pass for material uniforms.");
      return;
    }

    for (const auto &[id, material]: materials) {
      if (material->needsUpload && material->uniformBuffer.Valid()) {
        SDL_GPUTransferBufferLocation location{
          .transfer_buffer = material->uniformBuffer.transferBuffer,
          .offset = 0,
        };
        SDL_GPUBufferRegion region{
          .buffer = material->uniformBuffer.buffer,
          .offset = 0,
          .size = material->uniformBuffer.size,
        };

        SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
        material->needsUpload = false;
      }
    }

    SDL_EndGPUCopyPass(copyPass);
  }
} // namespace Engine
