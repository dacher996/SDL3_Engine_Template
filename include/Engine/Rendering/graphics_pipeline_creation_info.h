#ifndef ENGINE_GRAPHICS_PIPELINE_CREATION_INFO_H
#define ENGINE_GRAPHICS_PIPELINE_CREATION_INFO_H

#include "SDL3/SDL_gpu.h"

namespace Engine {
#define BLEND_STATE_DEFAULT SDL_GPUColorTargetBlendState { \
.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA, \
.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, \
.color_blend_op = SDL_GPU_BLENDOP_ADD, \
.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA, \
.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, \
.alpha_blend_op = SDL_GPU_BLENDOP_ADD, \
.enable_blend = true, \
}

    /// Creation parameters for a graphics pipeline
    struct GraphicsPipelineCreationInfo {
        SDL_GPUColorTargetBlendState blendState{BLEND_STATE_DEFAULT};
        SDL_GPUVertexInputState *vertexInputState{nullptr};
        SDL_GPUDepthStencilState *depthStencilState{nullptr};
    };
}

#endif //ENGINE_GRAPHICS_PIPELINE_CREATION_INFO_H
