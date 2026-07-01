#ifndef ENGINE_MOUSE_CODES_H
#define ENGINE_MOUSE_CODES_H

#include "SDL3/SDL_mouse.h"

namespace Engine {

    using MouseCode = int;

    namespace Mouse {
        constexpr MouseCode Left = SDL_BUTTON_LEFT;
        constexpr MouseCode Middle = SDL_BUTTON_MIDDLE;
        constexpr MouseCode Right = SDL_BUTTON_RIGHT;
        constexpr MouseCode X1 = SDL_BUTTON_X1;
        constexpr MouseCode X2 = SDL_BUTTON_X2;
    }
}

#endif //ENGINE_MOUSE_CODES_H
