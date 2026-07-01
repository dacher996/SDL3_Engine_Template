#include "Engine/Rendering/RenderPasses/imgui_render_pass.h"
#include "imgui_impl_sdlgpu3.h"

#include <imgui.h>

namespace Engine {
    ImGUIRenderPass::ImGUIRenderPass(SDL_GPUTexture *swapchainTexture) : RenderPass({}),
                                                                         m_swapchainTexture(swapchainTexture) {
    }

    void ImGUIRenderPass::PrepareData(FrameRenderData &frameRenderData) {
        // No-Op
    }

    bool ImGUIRenderPass::Render(SDL_GPUCommandBuffer *commandBuffer) {
        ImDrawData *drawData = ImGui::GetDrawData();
        if (!m_swapchainTexture || !drawData) return false;
        const bool isMinimised = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);

        if (isMinimised) return false;

        // This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
        ImGui_ImplSDLGPU3_PrepareDrawData(drawData, commandBuffer);

        // Setup and start a render pass
        SDL_GPUColorTargetInfo target_info = {};
        target_info.texture = m_swapchainTexture;
        target_info.load_op = SDL_GPU_LOADOP_LOAD;
        target_info.store_op = SDL_GPU_STOREOP_STORE;
        target_info.mip_level = 0;
        target_info.layer_or_depth_plane = 0;
        target_info.cycle = false;
        SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &target_info, 1, nullptr);

        // Render ImGui
        ImGui_ImplSDLGPU3_RenderDrawData(drawData, commandBuffer, renderPass);

        SDL_EndGPURenderPass(renderPass);
        return true;
    }
}
