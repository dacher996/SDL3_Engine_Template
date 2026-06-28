#ifndef ENGINE_RENDER_PASS_H
#define ENGINE_RENDER_PASS_H

#include "Engine/Rendering/texture.h"
#include "2D/scene_models_2d.h"
#include "Engine/Core/defines.h"

namespace Engine {
    struct RenderPassCreateInfo {
        /// Existing render target used by the render pass. If not provided, [renderTargetWidth] and [renderTargetHeight]
        /// will be used instead for creation and management of local render target
        Texture *renderTarget{nullptr};

        /// The width of the render target to be created
        Uint32 renderTargetWidth{LOGICAL_APP_WIDTH};

        /// The height of the render target to be created
        Uint32 renderTargetHeight{LOGICAL_APP_HEIGHT};
    };

    class RenderPass {
    public:
        explicit RenderPass(RenderPassCreateInfo createInfo);

        virtual ~RenderPass() = 0;

        /// Prepares render pass data for the current frame. Override in passes
        /// that need per-frame state (e.g. sprite draw batches, camera matrix).
        /// Default implementation is a no-op.
        virtual void PrepareData(FrameRenderData &frameRenderData) {
        }

        /// Issues a render pass. This is where the data is drawn.
        virtual bool Render(SDL_GPUCommandBuffer *commandBuffer) = 0;

        /// Returns the texture data of the render target.
        [[nodiscard]] virtual Texture *GetRenderTarget() const;

    private:
        /// Flag indicating whether the render target used for the render pass is managed locally
        bool m_manageRenderTargetLifecycle{false};

        /// The render target used for data rendering
        Texture *m_renderTarget;
    };
}

#endif //ENGINE_RENDER_PASS_H
