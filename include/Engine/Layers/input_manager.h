#ifndef ENGINE_INPUT_MANAGER_H
#define ENGINE_INPUT_MANAGER_H

#include "Engine/Core/layer_interface.h"
#include "Engine/Core/input_types.h"
#include "Engine/Core/datatypes.h"
#include "SDL3/SDL_events.h"
#include <unordered_map>
#include <vector>

namespace Engine {

    class InputManager : public ILayer {
    public:
        InputManager();
        ~InputManager();

        void OnUpdate(float dt);
        bool HandleEvent(SDL_Event* event);

        // State Polling (Used by static Input API)
        bool IsActionPressed(ActionID actionID, int gamepadIndex = 0) const;
        bool IsActionHeld(ActionID actionID, int gamepadIndex = 0) const;
        bool IsActionReleased(ActionID actionID, int gamepadIndex = 0) const;
        float GetActionAxis(ActionID actionID, int gamepadIndex = 0) const;

        bool IsKeyPressed(KeyCode key) const;
        bool IsKeyHeld(KeyCode key) const;
        bool IsModifierHeld(KeyCode modifier) const;
        bool IsMouseButtonPressed(MouseCode button) const;
        bool IsGamepadButtonPressed(GamepadCode button, int gamepadIndex = 0) const;
        float GetGamepadAxis(GamepadAxisCode axis, int gamepadIndex = 0) const;

        // Mouse & Cursor State
        Vec2f GetMousePosition() const;
        Vec2f GetGlobalMousePosition() const;
        Vec2f GetMouseDelta() const;
        Vec2f GetMouseScrollDelta() const;
        void SetCursorMode(CursorMode mode) const;
        void SetCursorPosition(float x, float y) const;

        // Binding Management
        void BindAction(ActionID actionID, const InputChord& chord);
        void UnbindAction(ActionID actionID);
        void ClearAllBindings();
        bool IsActionBound(ActionID actionID) const;

        // Rumble
        void RumbleGamepad(int gamepadIndex, float lowFrequency, float highFrequency, Uint32 durationMs) const;
        void RumbleGamepadTriggers(int gamepadIndex, float leftRumble, float rightRumble, Uint32 durationMs) const;
        void StopGamepadRumble(int gamepadIndex) const;

    private:
        struct InputState {
            bool pressed = false;
            bool held = false;
            bool released = false;
            float axisValue = 0.0f;
        };

        // Tracks physical device states
        std::unordered_map<KeyCode, InputState> m_keyStates;
        std::unordered_map<MouseCode, InputState> m_mouseStates;
        
        // Gamepad index -> (GamepadCode -> State)
        std::unordered_map<int, std::unordered_map<GamepadCode, InputState>> m_gamepadButtonStates;
        std::unordered_map<int, std::unordered_map<GamepadAxisCode, float>> m_gamepadAxes;
        
        // Maps an ActionID to a list of chords (OR relationship between chords, AND within chord)
        std::unordered_map<ActionID, std::vector<InputChord>> m_actionBindings;

        // Tracks the resolved state of each ActionID per gamepad index
        std::unordered_map<int, std::unordered_map<ActionID, InputState>> m_actionStates;

        // Mouse Delta Tracking
        float m_accumMouseDeltaX = 0.0f;
        float m_accumMouseDeltaY = 0.0f;
        float m_accumScrollX = 0.0f;
        float m_accumScrollY = 0.0f;
        
        float m_mouseDeltaX = 0.0f;
        float m_mouseDeltaY = 0.0f;
        float m_scrollDeltaX = 0.0f;
        float m_scrollDeltaY = 0.0f;

        // Opened SDL Gamepads
        std::unordered_map<SDL_JoystickID, SDL_Gamepad*> m_gamepads;
        std::unordered_map<SDL_JoystickID, int> m_joystickToGamepadIndex;
        int m_nextGamepadIndex = 0;

        void UpdateStateTransitions();
        void EvaluateActions();
        
        void AddGamepad(SDL_JoystickID joystickID);
        void RemoveGamepad(SDL_JoystickID joystickID);
        
        InputState& GetKeyState(KeyCode key);
        InputState& GetMouseState(MouseCode button);
        InputState& GetGamepadButtonState(int index, GamepadCode button);
    };

}

#endif //ENGINE_INPUT_MANAGER_H
