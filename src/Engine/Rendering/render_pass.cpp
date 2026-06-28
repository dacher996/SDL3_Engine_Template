#include "Engine/Rendering/render_pass.h"

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Layers/texture_manager.h"

namespace Engine {
    RenderPass::RenderPass(RenderPassCreateInfo createInfo) {
        if (!createInfo.renderTarget) {
            m_manageRenderTargetLifecycle = true;
            m_renderTarget =
                    TextureManager::CreateRenderTarget(createInfo.renderTargetWidth, createInfo.renderTargetHeight);
        } else {
            m_renderTarget = createInfo.renderTarget;
        }
    }

    RenderPass::~RenderPass() {
        if (m_manageRenderTargetLifecycle) {
            m_renderTarget->Free(App::GetLayer<AppContext>().gpuDevice);
            delete m_renderTarget;
        }
    }

    Texture *RenderPass::GetRenderTarget() const {
        return m_renderTarget;
    }
}
