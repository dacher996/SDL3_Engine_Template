#ifndef ENGINE_SCENE_MODELS_2D_H
#define ENGINE_SCENE_MODELS_2D_H

#include <vector>

#include "SDL3/SDL_stdinc.h"
#include <glm/glm.hpp>

namespace Engine {
    /// Structure used to define the smallest common state in a render pass when rendering entities.
    ///
    /// Entities with similar pipelines composed on them will be grouped together.
    struct PipelineStateDetails {
        /// Number of entities that share this state
        size_t itemCount{0};

        /// The material id used to work with the pipeline
        Uint16 materialId{0};
    };

    /// Render pass state data used to efficiently manage pipeline changes and optimize binding calls
    struct RenderPassStateData {
        /// Different pipeline states
        std::vector<PipelineStateDetails> data;

        /// Indexes of entities mapped in the sprite data SSBO
        std::vector<Uint32> entityIndexes;

        /// Total number of renderable entities
        size_t totalCount;

        /// Camera view matrix for this frame
        glm::mat4 viewMatrix{1.0f};
    };

    /// Per-view render data — one per camera view / SpriteSubmission.
    /// Groups the pipeline state with the viewport/scissor configuration
    /// and buffer offset needed to draw this view's portion of the shared buffers.
    struct ViewRenderData {
        /// Pipeline state groups and entity indices for this view
        RenderPassStateData stateData;

        /// GPU viewport rect for this view
        SDL_GPUViewport viewport{};

        /// Scissor rect for this view
        SDL_Rect scissor{};

        /// Starting vertex offset in the merged index buffer.
        /// Used as the first_vertex argument to SDL_DrawGPUPrimitives.
        int baseVertexIndex{0};
    };

    /// Complete frame render data containing all views to render in this frame.
    /// For single-view rendering, this contains exactly one ViewRenderData.
    /// For split-screen, it contains one per camera.
    struct FrameRenderData {
        std::vector<ViewRenderData> views;
    };
}

#endif //ENGINE_SCENE_MODELS_2D_H
