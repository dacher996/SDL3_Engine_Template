#ifndef ENGINE_KEY_CODES_H
#define ENGINE_KEY_CODES_H

#include "SDL3/SDL_scancode.h"

namespace Engine {
    using KeyCode = SDL_Scancode;

    namespace Key {
        // Alphanumeric keys
        constexpr KeyCode A = SDL_SCANCODE_A;
        constexpr KeyCode B = SDL_SCANCODE_B;
        constexpr KeyCode C = SDL_SCANCODE_C;
        constexpr KeyCode D = SDL_SCANCODE_D;
        constexpr KeyCode E = SDL_SCANCODE_E;
        constexpr KeyCode F = SDL_SCANCODE_F;
        constexpr KeyCode G = SDL_SCANCODE_G;
        constexpr KeyCode H = SDL_SCANCODE_H;
        constexpr KeyCode I = SDL_SCANCODE_I;
        constexpr KeyCode J = SDL_SCANCODE_J;
        constexpr KeyCode K = SDL_SCANCODE_K;
        constexpr KeyCode L = SDL_SCANCODE_L;
        constexpr KeyCode M = SDL_SCANCODE_M;
        constexpr KeyCode N = SDL_SCANCODE_N;
        constexpr KeyCode O = SDL_SCANCODE_O;
        constexpr KeyCode P = SDL_SCANCODE_P;
        constexpr KeyCode Q = SDL_SCANCODE_Q;
        constexpr KeyCode R = SDL_SCANCODE_R;
        constexpr KeyCode S = SDL_SCANCODE_S;
        constexpr KeyCode T = SDL_SCANCODE_T;
        constexpr KeyCode U = SDL_SCANCODE_U;
        constexpr KeyCode V = SDL_SCANCODE_V;
        constexpr KeyCode W = SDL_SCANCODE_W;
        constexpr KeyCode X = SDL_SCANCODE_X;
        constexpr KeyCode Y = SDL_SCANCODE_Y;
        constexpr KeyCode Z = SDL_SCANCODE_Z;

        // Numbers
        constexpr KeyCode Num0 = SDL_SCANCODE_0;
        constexpr KeyCode Num1 = SDL_SCANCODE_1;
        constexpr KeyCode Num2 = SDL_SCANCODE_2;
        constexpr KeyCode Num3 = SDL_SCANCODE_3;
        constexpr KeyCode Num4 = SDL_SCANCODE_4;
        constexpr KeyCode Num5 = SDL_SCANCODE_5;
        constexpr KeyCode Num6 = SDL_SCANCODE_6;
        constexpr KeyCode Num7 = SDL_SCANCODE_7;
        constexpr KeyCode Num8 = SDL_SCANCODE_8;
        constexpr KeyCode Num9 = SDL_SCANCODE_9;

        // Function keys
        constexpr KeyCode F1 = SDL_SCANCODE_F1;
        constexpr KeyCode F2 = SDL_SCANCODE_F2;
        constexpr KeyCode F3 = SDL_SCANCODE_F3;
        constexpr KeyCode F4 = SDL_SCANCODE_F4;
        constexpr KeyCode F5 = SDL_SCANCODE_F5;
        constexpr KeyCode F6 = SDL_SCANCODE_F6;
        constexpr KeyCode F7 = SDL_SCANCODE_F7;
        constexpr KeyCode F8 = SDL_SCANCODE_F8;
        constexpr KeyCode F9 = SDL_SCANCODE_F9;
        constexpr KeyCode F10 = SDL_SCANCODE_F10;
        constexpr KeyCode F11 = SDL_SCANCODE_F11;
        constexpr KeyCode F12 = SDL_SCANCODE_F12;

        // Special keys
        constexpr KeyCode Escape = SDL_SCANCODE_ESCAPE;
        constexpr KeyCode Enter = SDL_SCANCODE_RETURN;
        constexpr KeyCode Space = SDL_SCANCODE_SPACE;
        constexpr KeyCode Backspace = SDL_SCANCODE_BACKSPACE;
        constexpr KeyCode Tab = SDL_SCANCODE_TAB;

        // Punctuation
        constexpr KeyCode Grave = SDL_SCANCODE_GRAVE; // ` or ~
        constexpr KeyCode Minus = SDL_SCANCODE_MINUS; // - or _
        constexpr KeyCode Equals = SDL_SCANCODE_EQUALS; // = or +
        constexpr KeyCode LeftBracket = SDL_SCANCODE_LEFTBRACKET; // [ or {
        constexpr KeyCode RightBracket = SDL_SCANCODE_RIGHTBRACKET; // ] or }
        constexpr KeyCode Backslash = SDL_SCANCODE_BACKSLASH; // \ or |
        constexpr KeyCode Semicolon = SDL_SCANCODE_SEMICOLON; // ; or :
        constexpr KeyCode Apostrophe = SDL_SCANCODE_APOSTROPHE; // ' or "
        constexpr KeyCode Comma = SDL_SCANCODE_COMMA; // , or <
        constexpr KeyCode Period = SDL_SCANCODE_PERIOD; // . or >
        constexpr KeyCode Slash = SDL_SCANCODE_SLASH; // / or ?

        // Modifiers (as scancodes for layout-independent polling)
        constexpr KeyCode LeftShift = SDL_SCANCODE_LSHIFT; // Left Shift
        constexpr KeyCode RightShift = SDL_SCANCODE_RSHIFT; // Right Shift
        constexpr KeyCode LeftCtrl = SDL_SCANCODE_LCTRL; // Left Ctrl
        constexpr KeyCode RightCtrl = SDL_SCANCODE_RCTRL; // Right Ctrl
        constexpr KeyCode LeftAlt = SDL_SCANCODE_LALT; // Left Alt / Option
        constexpr KeyCode RightAlt = SDL_SCANCODE_RALT; // Right Alt / Option
        constexpr KeyCode LeftGUI = SDL_SCANCODE_LGUI; // Left Command / Windows / Super
        constexpr KeyCode RightGUI = SDL_SCANCODE_RGUI; // Right Command / Windows / Super

        // Arrows
        constexpr KeyCode Right = SDL_SCANCODE_RIGHT;
        constexpr KeyCode Left = SDL_SCANCODE_LEFT;
        constexpr KeyCode Down = SDL_SCANCODE_DOWN;
        constexpr KeyCode Up = SDL_SCANCODE_UP;
    } // namespace Key
} // namespace Engine

#endif // ENGINE_KEY_CODES_H
