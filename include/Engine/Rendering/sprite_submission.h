#ifndef ENGINE_SPRITE_SUBMISSION_H
#define ENGINE_SPRITE_SUBMISSION_H

#include <vector>

#include "2D/gpu_sprite.h"
#include "SDL3/SDL_gpu.h"

#include <glm/glm.hpp>

namespace Engine {
    /// A single sprite ready for GPU submission, combining the raw sprite data
    /// with sorting/pipeline metadata
    struct SpriteEntry {
        /// Sprite rendering data (position, size, UVs, color, rotation)
        GPUSprite data;

        /// Render layer depth — higher values appear in front
        Uint16 depth{0};

        /// Pipeline/shader id to use for rendering (0 = default)
        Uint16 shaderId{0};
    };

    /// Batch of sprites submitted by a scene (or any data source) for one frame.
    /// This is the contract between the data layer and the renderer.
    struct SpriteSubmission {
        /// All sprite entries to be rendered this frame
        std::vector<SpriteEntry> entries;

        /// The camera view matrix to use for this frame's rendering
        glm::mat4 viewMatrix{1.0f};

        /// Optional viewport — if hasViewport is false, the full render target is used
        SDL_GPUViewport viewport{0, 0, 0, 0, 0, 1};

        /// Optional scissor — if hasScissor is false, the full render target is used
        SDL_Rect scissor{0, 0, 0, 0};
    };
}

#endif //ENGINE_SPRITE_SUBMISSION_H
