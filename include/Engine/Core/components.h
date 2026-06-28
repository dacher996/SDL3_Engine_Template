#ifndef ENGINE_COMPONENTS_H
#define ENGINE_COMPONENTS_H

#include "datatypes.h"

namespace Engine {
    using Size = Vec2i;
    using Position = Vec3f;

    struct ShaderData {
        /// The id of the shader registered inside GraphicsPipelineManager
        Uint16 shaderId;
    };

    struct Shader {
        std::vector<ShaderData> shaders;
    };
}

#endif //ENGINE_COMPONENTS_H
