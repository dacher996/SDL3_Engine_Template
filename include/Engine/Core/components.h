#ifndef ENGINE_COMPONENTS_H
#define ENGINE_COMPONENTS_H

#include "datatypes.h"

#include <vector>

namespace Engine {
    using Size = Vec2i;
    using Position = Vec3f;

    struct MaterialComponent {
        std::vector<Uint16> materialIds;
    };
}

#endif //ENGINE_COMPONENTS_H
