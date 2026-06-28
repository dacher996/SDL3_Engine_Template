#ifndef ENGINE_TEXTURE_SAMPLER_MANAGER_H
#define ENGINE_TEXTURE_SAMPLER_MANAGER_H

#include <array>

#include "Engine/Core/layer_interface.h"
#include "Engine/Core/texture_sampler_type.h"
#include "SDL3/SDL_gpu.h"

namespace Engine {
    /// Stores and manages the lifecycle of texture samplers
    class TextureSamplerManager : ILayer {
    public:
        /// Returns a pointer to a texture sampler (allocating one if it doesn't exist)
        SDL_GPUSampler *GetSampler(TextureSamplerType type);

        /// Frees texture sampler of given type from memory
        void UnloadSampler(TextureSamplerType type);

        /// Frees all samplers from memory
        void Cleanup();

    private:
        std::array<SDL_GPUSampler *, 6> m_samplers = {};

        /// Builds sampler creation info for a given texture sampler
        static SDL_GPUSamplerCreateInfo GetSamplerCreateInfo(TextureSamplerType samplerType);
    };
}

#endif //ENGINE_TEXTURE_SAMPLER_MANAGER_H
