#ifndef ENGINE_GAMEPAD_CODES_H
#define ENGINE_GAMEPAD_CODES_H

#include "SDL3/SDL_gamepad.h"

namespace Engine {

    using GamepadCode = SDL_GamepadButton;
    using GamepadAxisCode = SDL_GamepadAxis;

    namespace Gamepad {
        // Buttons
        constexpr GamepadCode South = SDL_GAMEPAD_BUTTON_SOUTH; // A on Xbox
        constexpr GamepadCode East = SDL_GAMEPAD_BUTTON_EAST;   // B on Xbox
        constexpr GamepadCode West = SDL_GAMEPAD_BUTTON_WEST;   // X on Xbox
        constexpr GamepadCode North = SDL_GAMEPAD_BUTTON_NORTH; // Y on Xbox
        
        constexpr GamepadCode Back = SDL_GAMEPAD_BUTTON_BACK;
        constexpr GamepadCode Guide = SDL_GAMEPAD_BUTTON_GUIDE;
        constexpr GamepadCode Start = SDL_GAMEPAD_BUTTON_START;
        
        constexpr GamepadCode LeftStick = SDL_GAMEPAD_BUTTON_LEFT_STICK;
        constexpr GamepadCode RightStick = SDL_GAMEPAD_BUTTON_RIGHT_STICK;
        constexpr GamepadCode LeftShoulder = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
        constexpr GamepadCode RightShoulder = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
        
        constexpr GamepadCode DpadUp = SDL_GAMEPAD_BUTTON_DPAD_UP;
        constexpr GamepadCode DpadDown = SDL_GAMEPAD_BUTTON_DPAD_DOWN;
        constexpr GamepadCode DpadLeft = SDL_GAMEPAD_BUTTON_DPAD_LEFT;
        constexpr GamepadCode DpadRight = SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
        
        // Axes
        constexpr GamepadAxisCode AxisLeftX = SDL_GAMEPAD_AXIS_LEFTX;
        constexpr GamepadAxisCode AxisLeftY = SDL_GAMEPAD_AXIS_LEFTY;
        constexpr GamepadAxisCode AxisRightX = SDL_GAMEPAD_AXIS_RIGHTX;
        constexpr GamepadAxisCode AxisRightY = SDL_GAMEPAD_AXIS_RIGHTY;
        constexpr GamepadAxisCode AxisLeftTrigger = SDL_GAMEPAD_AXIS_LEFT_TRIGGER;
        constexpr GamepadAxisCode AxisRightTrigger = SDL_GAMEPAD_AXIS_RIGHT_TRIGGER;
    }
}

#endif //ENGINE_GAMEPAD_CODES_H
