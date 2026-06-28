#ifndef ENGINE_TEXTURE_SAMPLER_TYPE_H
#define ENGINE_TEXTURE_SAMPLER_TYPE_H

#include <cstdint>

namespace Engine {
    /// Supported texture sampler types
    enum TextureSamplerType : uint8_t {
        POINT_CLAMP = 0,
        POINT_WRAP,
        LINEAR_CLAMP,
        LINEAR_WRAP,
        ANISOTROPIC_CLAMP,
        ANISOTROPIC_WRAP,
    };
}

#endif //ENGINE_TEXTURE_SAMPLER_TYPE_H
