#ifndef ENGINE_INPUT_TYPES_H
#define ENGINE_INPUT_TYPES_H

#include "Engine/Core/key_codes.h"
#include "Engine/Core/mouse_codes.h"
#include "Engine/Core/gamepad_codes.h"
#include <variant>

namespace Engine {

    using ActionID = Uint32;

    enum class InputDeviceType {
        Keyboard,
        MouseButton,
        GamepadButton,
        GamepadAxis
    };

    struct InputBinding {
        InputDeviceType deviceType;
        std::variant<KeyCode, MouseCode, GamepadCode, GamepadAxisCode> code;
        float deadzone = 0.1f; // For Gamepad Axis

        // Convenience constructors
        InputBinding(KeyCode key) : deviceType(InputDeviceType::Keyboard), code(key) {}
        InputBinding(MouseCode mouseBtn, bool isMouse = true) : deviceType(InputDeviceType::MouseButton), code(mouseBtn) {}
        InputBinding(GamepadCode gpBtn) : deviceType(InputDeviceType::GamepadButton), code(gpBtn) {}
        InputBinding(GamepadAxisCode gpAxis, float axisDeadzone = 0.1f) : deviceType(InputDeviceType::GamepadAxis), code(gpAxis), deadzone(axisDeadzone) {}
    };

    struct InputChord {
        std::vector<InputBinding> bindings;

        InputChord() = default;
        InputChord(const InputBinding& binding) { bindings.push_back(binding); }
        InputChord(std::initializer_list<InputBinding> list) : bindings(list) {}

        void AddBinding(const InputBinding& binding) { bindings.push_back(binding); }
    };

    enum class CursorMode {
        Normal,
        Hidden,
        Locked
    };

}
#endif //ENGINE_INPUT_TYPES_H
