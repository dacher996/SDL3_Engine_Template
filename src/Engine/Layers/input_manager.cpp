#include "Engine/Layers/input_manager.h"
#include "Engine/Core/app.h"
#include "Engine/Core/logger.h"
#include "Engine/Layers/scene_manager.h"
#include "Engine/Core/app_context.h"
#include "SDL3/SDL_joystick.h"
#include "SDL3/SDL_mouse.h"
#include <algorithm>

namespace Engine {
  InputManager::InputManager() {
    // Initialize connected gamepads
    int count;
    if (SDL_JoystickID *joysticks = SDL_GetGamepads(&count)) {
      for (int i = 0; i < count; ++i) {
        AddGamepad(joysticks[i]);
      }
      SDL_free(joysticks);
    }
  }

  InputManager::~InputManager() {
    for (auto &pair: m_gamepads) {
      SDL_CloseGamepad(pair.second);
    }
    m_gamepads.clear();
  }

  void InputManager::OnUpdate(float dt) {
    UpdateStateTransitions();
    EvaluateActions();
  }

  void InputManager::UpdateStateTransitions() {
    auto updateState = [](InputState &state) {
      if (state.pressed) {
        state.pressed = false;
        state.held = true;
      }
      if (state.released) {
        state.released = false;
        state.held = false;
      }
    };

    for (auto &pair: m_keyStates)
      updateState(pair.second);
    for (auto &pair: m_mouseStates)
      updateState(pair.second);
    for (auto &gamepadPair: m_gamepadButtonStates) {
      for (auto &btnPair: gamepadPair.second) {
        updateState(btnPair);
      }
    }
    for (auto &gamepadPair: m_actionStates) {
      for (auto &actPair: gamepadPair.second) {
        updateState(actPair.second);
      }
    }

    m_mouseDeltaX = m_accumMouseDeltaX;
    m_mouseDeltaY = m_accumMouseDeltaY;
    m_scrollDeltaX = m_accumScrollX;
    m_scrollDeltaY = m_accumScrollY;

    m_accumMouseDeltaX = 0.0f;
    m_accumMouseDeltaY = 0.0f;
    m_accumScrollX = 0.0f;
    m_accumScrollY = 0.0f;
  }

  void InputManager::EvaluateActions() {
    // We will evaluate actions from current hardware state
    // If an action transitions to true, we fire an event if not already held.
    // Wait, for simplicity, we evaluate actions inside the OnUpdate to keep state
    // correct, but AppEvent dispatch for actions should ideally happen as soon as
    // the hardware event arrives, OR we dispatch it here during OnUpdate. Firing
    // during hardware event (HandleEvent) is better for latency. The evaluate
    // here ensures hold states for actions are correct.

    // Reset all action evaluations
    for (auto &gamepadPair: m_actionStates) {
      for (auto &actPair: gamepadPair.second) {
        // If it was pressed/held, assume not pressed this frame until proven
        // otherwise, but wait, we already track transitions. It's better to
        // update action state directly in HandleEvent, OR we just compute the raw
        // 'is active' from hardware, and manage transitions.
        for (auto &gamepadPair: m_actionStates) {
          for (auto &actPair: gamepadPair.second) {
            bool isActive = false;
            float highestAxis = 0.0f;
            int gpadIdx = gamepadPair.first;

            auto it = m_actionBindings.find(actPair.first);
            if (it != m_actionBindings.end()) {
              for (const auto &chord: it->second) {
                bool chordActive = true;
                float chordAxis = 0.0f;

                for (const auto &binding: chord.bindings) {
                  bool bindingActive = false;
                  switch (binding.deviceType) {
                    case InputDeviceType::Keyboard:
                      if (IsKeyHeld(std::get<KeyCode>(binding.code)) ||
                          IsKeyPressed(std::get<KeyCode>(binding.code)))
                        bindingActive = true;
                      break;
                    case InputDeviceType::MouseButton:
                      if (IsMouseButtonPressed(std::get<MouseCode>(binding.code)))
                        bindingActive = true;
                      {
                        auto m_it = m_mouseStates.find(std::get<MouseCode>(binding.code));
                        if (m_it != m_mouseStates.end() &&
                            (m_it->second.pressed || m_it->second.held))
                          bindingActive = true;
                      }
                      break;
                    case InputDeviceType::GamepadButton: {
                      auto b_it = m_gamepadButtonStates[gpadIdx].find(
                        std::get<GamepadCode>(binding.code));
                      if (b_it != m_gamepadButtonStates[gpadIdx].end() &&
                          (b_it->second.pressed || b_it->second.held))
                        bindingActive = true;
                    }
                    break;
                    case InputDeviceType::GamepadAxis: {
                      float axis = GetGamepadAxis(std::get<GamepadAxisCode>(binding.code),
                                                  gpadIdx);
                      if (std::abs(axis) >= binding.deadzone) {
                        bindingActive = true;
                        if (std::abs(axis) > std::abs(chordAxis))
                          chordAxis = axis;
                      }
                    }
                    break;
                  }

                  if (!bindingActive) {
                    chordActive = false;
                    break;
                  }
                }

                if (chordActive && !chord.bindings.empty()) {
                  isActive = true;
                  if (std::abs(chordAxis) > std::abs(highestAxis))
                    highestAxis = chordAxis;
                }
              }
            }

            auto &state = actPair.second;
            bool wasActive = state.pressed || state.held;

            if (isActive && !wasActive) {
              state.pressed = true;
              state.released = false;
              App::GetLayer<SceneManager>().OnEvent(
                static_cast<AppEvent>(ActionPressedEvent(actPair.first, gpadIdx)));
            } else if (!isActive && wasActive) {
              state.pressed = false;
              state.released = true;
              App::GetLayer<SceneManager>().OnEvent(
                static_cast<AppEvent>(ActionReleasedEvent(actPair.first, gpadIdx)));
            }

            state.axisValue = highestAxis;
          }
        }
      }

      bool InputManager::HandleEvent(SDL_Event *event)
      {
        switch (event->type) {
          case SDL_EVENT_KEY_DOWN: {
            if (!event->key.repeat) {
              GetKeyState(event->key.scancode).pressed = true;
              GetKeyState(event->key.scancode).released = false;
              GetKeyState(event->key.scancode).held = false;
            }
            break;
          }
          case SDL_EVENT_KEY_UP: {
            GetKeyState(event->key.scancode).released = true;
            GetKeyState(event->key.scancode).pressed = false;
            GetKeyState(event->key.scancode).held = false;
            break;
          }
          case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            GetMouseState(event->button.button).pressed = true;
            GetMouseState(event->button.button).released = false;
            GetMouseState(event->button.button).held = false;
            break;
          }
          case SDL_EVENT_MOUSE_BUTTON_UP: {
            GetMouseState(event->button.button).released = true;
            GetMouseState(event->button.button).pressed = false;
            GetMouseState(event->button.button).held = false;
            break;
          }
          case SDL_EVENT_MOUSE_MOTION: {
            m_accumMouseDeltaX += event->motion.xrel;
            m_accumMouseDeltaY += event->motion.yrel;
            break;
          }
          case SDL_EVENT_MOUSE_WHEEL: {
            m_accumScrollX += event->wheel.x;
            m_accumScrollY += event->wheel.y;
            break;
          }
          case SDL_EVENT_GAMEPAD_ADDED: {
            AddGamepad(event->gdevice.which);
            break;
          }
          case SDL_EVENT_GAMEPAD_REMOVED: {
            RemoveGamepad(event->gdevice.which);
            break;
          }
          case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
            int gpadIdx = m_joystickToGamepadIndex[event->gbutton.which];
            GetGamepadButtonState(gpadIdx, (GamepadCode) event->gbutton.button).pressed =
                true;
            GetGamepadButtonState(gpadIdx, (GamepadCode) event->gbutton.button)
                .released = false;
            GetGamepadButtonState(gpadIdx, (GamepadCode) event->gbutton.button).held =
                false;
            break;
          }
          case SDL_EVENT_GAMEPAD_BUTTON_UP: {
            int gpadIdx = m_joystickToGamepadIndex[event->gbutton.which];
            GetGamepadButtonState(gpadIdx, static_cast<GamepadCode>(event->gbutton.button))
                .released = true;
            GetGamepadButtonState(gpadIdx, static_cast<GamepadCode>(event->gbutton.button)).pressed =
                false;
            GetGamepadButtonState(gpadIdx, static_cast<GamepadCode>(event->gbutton.button)).held =
                false;
            break;
          }
          case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
            int gpadIdx = m_joystickToGamepadIndex[event->gaxis.which];
            float val = event->gaxis.value / 32767.0f; // roughly normalize to -1.0 to 1.0
            m_gamepadAxes[gpadIdx][static_cast<GamepadAxisCode>(event->gaxis.axis)] = val;
            break;
          }
          default: ;
        }
        return false;
      }

      void InputManager::AddGamepad(SDL_JoystickID joystickID)
      {
        if (m_gamepads.find(joystickID) != m_gamepads.end())
          return;
        SDL_Gamepad *gamepad = SDL_OpenGamepad(joystickID);
        if (gamepad) {
          m_gamepads[joystickID] = gamepad;
          int gpadIdx = m_nextGamepadIndex++;
          m_joystickToGamepadIndex[joystickID] = gpadIdx;
          ENGINE_LOG_INFO("Gamepad added: Index {}", gpadIdx);
          App::GetLayer<SceneManager>().OnEvent(static_cast<AppEvent>(GamepadConnectedEvent(gpadIdx)));

          // Initialize action states for this gamepad
          for (const auto &pair: m_actionBindings) {
            m_actionStates[gpadIdx][pair.first] = InputState{};
          }
        }
      }

      void InputManager::RemoveGamepad(SDL_JoystickID joystickID)
      {
        auto it = m_gamepads.find(joystickID);
        if (it != m_gamepads.end()) {
          SDL_CloseGamepad(it->second);
          m_gamepads.erase(it);
          int gpadIdx = m_joystickToGamepadIndex[joystickID];
          m_joystickToGamepadIndex.erase(joystickID);
          ENGINE_LOG_INFO("Gamepad removed: Index {}", gpadIdx);
          App::GetLayer<SceneManager>().OnEvent(static_cast<AppEvent>(GamepadDisconnectedEvent(gpadIdx)));
        }
      }

      InputManager::InputState &InputManager::GetKeyState(KeyCode key)
      {
        return m_keyStates[key];
      }

      InputManager::InputState &InputManager::GetMouseState(MouseCode button)
      {
        return m_mouseStates[button];
      }

      InputManager::InputState &
          InputManager::GetGamepadButtonState(int index, GamepadCode button)
      {
        return m_gamepadButtonStates[index][button];
      }

      void InputManager::BindAction(ActionID actionID, const InputChord &chord)
      {
        m_actionBindings[actionID].push_back(chord);
        // Ensure action state exists for gamepad 0 (keyboard/mouse primary)
        m_actionStates[0][actionID] = InputState{};
        for (const auto &pair: m_gamepads) {
          m_actionStates[m_joystickToGamepadIndex[pair.first]][actionID] =
              InputState{};
        }
      }

      void InputManager::UnbindAction(ActionID actionID)
      {
        m_actionBindings.erase(actionID);
      }

      void InputManager::ClearAllBindings()
      {
        m_actionBindings.clear();
      }

      bool InputManager::IsActionBound(ActionID actionID) const
      {
        auto it = m_actionBindings.find(actionID);
        return it != m_actionBindings.end() && !it->second.empty();
      }

      Vec2f InputManager::GetMousePosition() const
      {
        float x, y;
        SDL_GetMouseState(&x, &y);
        return Vec2f(x, y);
      }

      Vec2f InputManager::GetGlobalMousePosition() const
      {
        float x, y;
        SDL_GetGlobalMouseState(&x, &y);
        return Vec2f(x, y);
      }

      Vec2f InputManager::GetMouseDelta() const
      {
        return Vec2f(m_mouseDeltaX, m_mouseDeltaY);
      }

      Vec2f InputManager::GetMouseScrollDelta() const
      {
        return Vec2f(m_scrollDeltaX, m_scrollDeltaY);
      }

      void InputManager::SetCursorMode(CursorMode mode) const
      {
        SDL_Window *window = App::GetLayer<AppContext>().window;
        switch (mode) {
          case CursorMode::Normal:
            SDL_ShowCursor();
            SDL_SetWindowRelativeMouseMode(window, false);
            break;
          case CursorMode::Hidden:
            SDL_HideCursor();
            SDL_SetWindowRelativeMouseMode(window, false);
            break;
          case CursorMode::Locked:
            SDL_SetWindowRelativeMouseMode(window, true);
            break;
        }
      }

      void InputManager::SetCursorPosition(float x, float y) const
      {
        SDL_Window *window = App::GetLayer<AppContext>().window;
        SDL_WarpMouseInWindow(window, x, y);
      }

      bool InputManager::IsActionPressed(ActionID actionID, int gamepadIndex) const
      {
        if (auto it1 = m_actionStates.find(gamepadIndex); it1 != m_actionStates.end()) {
          if (auto it2 = it1->second.find(actionID); it2 != it1->second.end())
            return it2->second.pressed;
        }
        return false;
      }

      bool InputManager::IsActionHeld(ActionID actionID, int gamepadIndex) const
      {
        if (auto it1 = m_actionStates.find(gamepadIndex); it1 != m_actionStates.end()) {
          if (auto it2 = it1->second.find(actionID); it2 != it1->second.end())
            return it2->second.held || it2->second.pressed;
        }
        return false;
      }

      bool InputManager::IsActionReleased(ActionID actionID, int gamepadIndex) const
      {
        if (auto it1 = m_actionStates.find(gamepadIndex); it1 != m_actionStates.end()) {
          if (auto it2 = it1->second.find(actionID); it2 != it1->second.end())
            return it2->second.released;
        }
        return false;
      }

      float InputManager::GetActionAxis(ActionID actionID, int gamepadIndex) const
      {
        if (auto it1 = m_actionStates.find(gamepadIndex); it1 != m_actionStates.end()) {
          if (auto it2 = it1->second.find(actionID); it2 != it1->second.end())
            return it2->second.axisValue;
        }
        return 0.0f;
      }

      bool InputManager::IsKeyPressed(KeyCode key) const
      {
        auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && it->second.pressed;
      }

      bool InputManager::IsKeyHeld(KeyCode key) const
      {
        auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && (it->second.held || it->second.pressed);
      }

      bool InputManager::IsModifierHeld(KeyCode modifier) const
      {
        return IsKeyHeld(modifier);
      }

      bool InputManager::IsMouseButtonPressed(MouseCode button) const
      {
        auto it = m_mouseStates.find(button);
        return it != m_mouseStates.end() && it->second.pressed;
      }

      bool InputManager::IsGamepadButtonPressed(GamepadCode button,
                                                int gamepadIndex) const
      {
        if (auto it1 = m_gamepadButtonStates.find(gamepadIndex); it1 != m_gamepadButtonStates.end()) {
          if (auto it2 = it1->second.find(button); it2 != it1->second.end())
            return it2->second.pressed;
        }
        return false;
      }

      float InputManager::GetGamepadAxis(GamepadAxisCode axis,
                                         int gamepadIndex) const
      {
        if (auto it1 = m_gamepadAxes.find(gamepadIndex); it1 != m_gamepadAxes.end()) {
          if (auto it2 = it1->second.find(axis); it2 != it1->second.end())
            return it2->second;
        }
        return 0.0f;
      }

      void InputManager::RumbleGamepad(int gamepadIndex, float lowFrequency, float highFrequency,
                                       Uint32 durationMs) const
      {
        for (const auto &pair: m_joystickToGamepadIndex) {
          if (pair.second == gamepadIndex) {
            auto it = m_gamepads.find(pair.first);
            if (it != m_gamepads.end()) {
              SDL_RumbleGamepad(it->second,
                                static_cast<Uint16>(std::clamp(lowFrequency, 0.0f, 1.0f) * 0xFFFF),
                                static_cast<Uint16>(std::clamp(highFrequency, 0.0f, 1.0f) * 0xFFFF),
                                durationMs);
            }
            break;
          }
        }
      }

      void InputManager::RumbleGamepadTriggers(int gamepadIndex, float leftRumble, float rightRumble,
                                               Uint32 durationMs) const
      {
        for (const auto &pair: m_joystickToGamepadIndex) {
          if (pair.second == gamepadIndex) {
            auto it = m_gamepads.find(pair.first);
            if (it != m_gamepads.end()) {
              SDL_RumbleGamepadTriggers(it->second,
                                        static_cast<Uint16>(std::clamp(leftRumble, 0.0f, 1.0f) * 0xFFFF),
                                        static_cast<Uint16>(std::clamp(rightRumble, 0.0f, 1.0f) * 0xFFFF),
                                        durationMs);
            }
            break;
          }
        }
      }

      void InputManager::StopGamepadRumble(int gamepadIndex) const
      {
        RumbleGamepad(gamepadIndex, 0.0f, 0.0f, 0);
        RumbleGamepadTriggers(gamepadIndex, 0.0f, 0.0f, 0);
      }
    } // namespace Engine
