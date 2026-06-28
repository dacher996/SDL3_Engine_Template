#ifndef APP_COMPONENTS_H
#define APP_COMPONENTS_H

#include "Engine/Core/color.h"
#include "Engine/Core/texture_region.h"

namespace YourProject {
    struct Sprite {
        float rotation{0};
        Engine::Color color{Engine::Color::WHITE};
        Engine::TextureRegion textureData{};
    };
}

#endif //APP_COMPONENTS_H
