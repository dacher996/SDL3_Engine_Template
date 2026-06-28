#ifndef APP_COMPONENTS_H
#define APP_COMPONENTS_H

#include "Engine/Core/color.h"

namespace YourProject {
    /// Sprite component
    struct Sprite {
        /// The key representing a texture region within the TextureRegionManager
        Uint32 spriteId;

        /// The rotation of the sprite
        float rotation{0};

        /// The color of the sprite
        Engine::Color color{Engine::Color::WHITE};
    };
}

#endif //APP_COMPONENTS_H
