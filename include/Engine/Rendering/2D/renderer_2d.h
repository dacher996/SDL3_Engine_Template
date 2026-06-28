#ifndef ENGINE_RENDERER_2D_H
#define ENGINE_RENDERER_2D_H

#include "default_2d_render_pass.h"
#include "scene_models_2d.h"
#include "Engine/Rendering/render_pass.h"
#include "Engine/Rendering/sprite_submission.h"
#include "Engine/Rendering/storage_buffer.h"

namespace Engine {
    /// 2D sprite renderer that owns the GPU buffers (single source of truth) and
/// orchestrates the full rendering pipeline: sorting, buffer upload, render
/// pass execution, and final presentation to the swapchain.
///
/// The renderer is scene-agnostic — it accepts sprite data via Submit() and
/// can be used by any data source (ECS scenes, UI layers, particle systems).
///
/// The rendering pipeline is customizable. By default it uses a 2-pass flow
/// (offscreen sprite pass → swapchain presentation), but users can set a
/// custom ordered list of content passes via SetRenderPipeline().
///
/// Multiple SpriteSubmissions can be submitted per frame for multi-view
/// rendering (e.g. split-screen). Each submission carries its own view matrix
/// and optional viewport/scissor configuration.
    class Renderer2D {
    public:
        /// Creates the renderer with the given offscreen render target dimensions.
        /// These dimensions are used for the default render pass and as the default
        /// viewport/scissor when submissions don't specify custom values.
        explicit Renderer2D();

        ~Renderer2D();

        /// Submit a single view's sprite data for the current frame.
        /// Equivalent to Submit({submission}).
        void Submit(const SpriteSubmission &submission);

        /// Submit multiple views' sprite data for the current frame.
        /// Each submission represents a separate camera view with its own view matrix
        /// and optional viewport/scissor. Sprite data from all submissions is merged
        /// into the shared GPU buffers with proper offsets.
        void Submit(const std::vector<SpriteSubmission> &submissions);

        /// Execute the full rendering pipeline:
        /// 1. Acquire command buffer + swapchain texture
        /// 2. Upload transfer buffers to GPU (copy pass)
        /// 3. Execute all content passes in order (each iterates over all views)
        /// 4. Blit the last pass's render target to the swapchain (present)
        /// 5. Submit command buffer
        void Render();
        
        /// Set a custom ordered list of content render passes. Replaces the default
        /// pipeline. Pass ownership is NOT transferred (caller manages lifetime).
        /// The last pass's render target will be used as the source for the final
        /// swapchain presentation.
        void SetRenderPipeline(std::vector<RenderPass *> passes);

        /// Reset to the default 2-pass pipeline (Default2DRenderPass → present)
        void UseDefaultPipeline();

        /// Access the shared sprite data GPU buffer (for custom render passes that
        /// need to bind it)
        [[nodiscard]] StorageBuffer &GetSpriteDataBuffer();

        /// Access the shared entity index GPU buffer (for custom render passes that
        /// need to bind it)
        [[nodiscard]] StorageBuffer &GetEntityIndexBuffer();

    private:
        /// Sprite data buffer — THE single source of truth for all sprite data
        StorageBuffer m_spriteDataBuffer;

        /// Entity index buffer — sorted indices into sprite data buffer
        StorageBuffer m_entityIndexBuffer;

        /// Current frame's render data containing all views
        FrameRenderData m_currentFrameData;

        /// Ordered list of content render passes to execute each frame
        std::vector<RenderPass *> m_renderPasses;

        /// Whether we're currently using the internally-managed default pipeline
        bool m_usingDefaultPipeline{true};

        /// Default 2D sprite render pass (owned when using default pipeline)
        Default2DRenderPass *m_defaultPass{nullptr};

        /// Processes multiple submissions into merged GPU buffers and per-view
        /// render state. Each view's sprite data is packed contiguously into the
        /// shared buffers with index values offset accordingly.
        FrameRenderData
        PrepareFrame(const std::vector<SpriteSubmission> &submissions);

        /// Uploads both transfer buffers (sprite data + indices) to GPU via a copy
        /// pass.
        void UploadBuffers(SDL_GPUCommandBuffer *commandBuffer) const;
    };
}

#endif //ENGINE_RENDERER_2D_H
