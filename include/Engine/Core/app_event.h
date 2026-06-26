#ifndef ENGINE_APP_EVENT_H
#define ENGINE_APP_EVENT_H

#include "SDL3/SDL_events.h"

namespace Engine {
    struct AppEvent {
    };

    /// An app event coming from SDL
    struct SDLEvent : AppEvent {
        SDLEvent(SDL_Event *e) : event(e) {
        }

        SDL_Event *event;
    };

    /// Event issued once the current window has finished resizing
    struct WindowResizedEvent : AppEvent {
        WindowResizedEvent(int newWidth, int newHeight) : width(newWidth), height(newHeight) {
        };

        int width;
        int height;
    };

    enum WindowMode { WINDOWED, BORDERLESS, FULLSCREEN };

    /// Event issued once the fullscreen state has changed
    struct FullscreenChanged : AppEvent {
        FullscreenChanged(WindowMode mode) : mode(mode) {
        }

        WindowMode mode;
    };
}

#endif //ENGINE_APP_EVENT_H
