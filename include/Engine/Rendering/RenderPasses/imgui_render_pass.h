#ifndef ENGINE_IMGUI_RENDER_PASS_H
#define ENGINE_IMGUI_RENDER_PASS_H

#include "../render_pass.h"

namespace Engine {
    class ImGUIRenderPass : public RenderPass {
    public:
        ImGUIRenderPass(SDL_GPUTexture *swapchainTexture);

        void PrepareData(FrameRenderData &frameRenderData) override;

        bool Render(SDL_GPUCommandBuffer *commandBuffer) override;

    private:
        /// The swapchain texture representing the target screen buffer to draw to.
        SDL_GPUTexture *m_swapchainTexture;
    };
}

#endif //ENGINE_IMGUI_RENDER_PASS_H
