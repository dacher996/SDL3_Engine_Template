#ifndef ENGINE_DEFAULT_2D_RENDER_PASS_H
#define ENGINE_DEFAULT_2D_RENDER_PASS_H

#include "Engine/Rendering/render_pass.h"
#include "scene_models_2d.h"
#include "Engine/Rendering/storage_buffer.h"

namespace Engine {
    class Default2DRenderPass : public RenderPass {
    public:
        Default2DRenderPass(StorageBuffer &spriteData, StorageBuffer &entityIndexData,
                            Uint32 renderTargetWidth = 800, Uint32 renderTargetHeight = 600);

        ~Default2DRenderPass() override = default;

        void PrepareData(FrameRenderData &frameRenderData) override;

        bool Render(SDL_GPUCommandBuffer *commandBuffer) override;

    private:
        StorageBuffer *m_spriteDataBuffer;
        StorageBuffer *m_entityIndexBuffer;
        FrameRenderData *m_frameData;
    };
}

#endif //ENGINE_DEFAULT_2D_RENDER_PASS_H
