#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H

#include "Engine/Core/input_types.h"
#include "Engine/Core/datatypes.h"

namespace Engine {

    class Input {
    public:
        // Polling
        static bool IsActionPressed(ActionID actionID, int gamepadIndex = 0);
        static bool IsActionHeld(ActionID actionID, int gamepadIndex = 0);
        static bool IsActionReleased(ActionID actionID, int gamepadIndex = 0);
        static float GetActionAxis(ActionID actionID, int gamepadIndex = 0);

        static bool IsKeyPressed(KeyCode key);
        static bool IsKeyHeld(KeyCode key);
        static bool IsModifierHeld(KeyCode modifier);
        static bool IsMouseButtonPressed(MouseCode button);
        static bool IsGamepadButtonPressed(GamepadCode button, int gamepadIndex = 0);
        static float GetGamepadAxis(GamepadAxisCode axis, int gamepadIndex = 0);

        // Mouse & Cursor State
        static Vec2f GetMousePosition();
        static Vec2f GetGlobalMousePosition();
        static Vec2f GetMouseDelta();
        static Vec2f GetMouseScrollDelta();
        static void SetCursorMode(CursorMode mode);
        static void SetCursorPosition(float x, float y);

        // Binding Management
        static void BindAction(ActionID actionID, const InputChord& chord);
        static void UnbindAction(ActionID actionID);
        static void ClearAllBindings();
        static bool IsActionBound(ActionID actionID);

        // Rumble
        static void RumbleGamepad(int gamepadIndex, float lowFrequency, float highFrequency, Uint32 durationMs);
        static void RumbleGamepadTriggers(int gamepadIndex, float leftRumble, float rightRumble, Uint32 durationMs);
        static void StopGamepadRumble(int gamepadIndex);
    };

}

#endif //ENGINE_INPUT_H
