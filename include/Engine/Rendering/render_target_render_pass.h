#ifndef ENGINE_RENDER_TARGET_RENDER_PASS_H
#define ENGINE_RENDER_TARGET_RENDER_PASS_H

#include <array>

#include "logical_presentation.h"
#include "render_pass.h"
#include "SDL3/SDL_gpu.h"

namespace Engine {
    class TextureToScreenRenderPass : public RenderPass {
    public:
        /// Constructor that sets up the pass with the swapchain texture (destination)
        /// and the render target texture (source).
        explicit TextureToScreenRenderPass(SDL_GPUTexture *swapchainTexture, Texture *renderTarget,
                                           LogicalPresentation mode = LogicalPresentation::Overscan);

        /// Main render function that binds the pipeline, computes the aspect-corrected
        /// screen rectangle, pushes it as uniform data, and issues the draw call.
        bool Render(SDL_GPUCommandBuffer *commandBuffer) override;

    private:
        /// Calculates the normalized destination rectangle [x, y, w, h] on the screen.
        /// It fits the render target within the window using aspect-ratio-corrected letterboxing.
        /// Values returned are in normalized screen space coordinates [0.0, 1.0].
        [[nodiscard]] std::array<float, 4> CalculateScreenRect(LogicalPresentation mode) const;

        /// The swapchain texture representing the target screen buffer to draw to.
        SDL_GPUTexture *m_swapchainTexture;

        /// The scaling mode of the render target
        LogicalPresentation m_presentationMode;
    };
}

#endif //ENGINE_RENDER_TARGET_RENDER_PASS_H
