#ifndef ENGINE_TEXTURE_REGION_H
#define ENGINE_TEXTURE_REGION_H

#include "SDL3/SDL_stdinc.h"

namespace Engine {
    /// Texture region
    struct TextureRegion {
        /// The id of the texture registered inside TextureManager
        Uint8 textureId;

        /// The layer id within which the texture region is located
        Uint8 layerId;

        /// The normalized coordinates of the top-left position of the region
        float x, y;

        /// The normalized size of the region
        float w, h;
    };
}

#endif //ENGINE_TEXTURE_REGION_H
