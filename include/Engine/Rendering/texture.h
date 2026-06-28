#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H

#include "Engine/Core/texture_sampler_type.h"
#include "SDL3/SDL_gpu.h"

namespace Engine {
    /// Structure that contains information about an array texture.
    ///
    /// Note: All layers of the array texture should have the same dimensions.
    struct Texture {
        /// The pointer towards the GPU texture
        SDL_GPUTexture *texture{nullptr};

        /// Texture dimensions
        Uint32 width{0}, height{0};

        /// Number of textures stored inside the array texture
        int layers{0};

        /// Sampler info used on this texture
        TextureSamplerType samplerHint{LINEAR_CLAMP};

        /// Retrieves the size of an individual layer
        [[nodiscard]] Uint32 GetTextureSize() const { return width * height * 4; }

        [[nodiscard]] bool Valid() const { return texture != nullptr; }

        void Free(SDL_GPUDevice *gpuDevice) {
            if (texture) {
                SDL_ReleaseGPUTexture(gpuDevice, texture);
                texture = nullptr;
            }
            width = height = layers = 0;
        }
    };
}

#endif //ENGINE_TEXTURE_H
