#include "Engine/app.h"
#include "Engine/logger.h"

Engine::App::App(int argc, char *argv[]) : m_window{nullptr, &SDL_DestroyWindow} {
    Logger::Init();
}

SDL_AppResult Engine::App::Init() {
    SDL_Init(SDL_INIT_VIDEO);

    m_window.reset(SDL_CreateWindow("Project name", 800, 600,SDL_WINDOW_RESIZABLE));

    return SDL_APP_CONTINUE;
}

SDL_AppResult Engine::App::Event(SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (SDL_GetWindowID(m_window.get()) == event->window.windowID) {
                return SDL_APP_SUCCESS;
            }
#ifdef DEBUG_MODE
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_ESCAPE) {
                return SDL_APP_SUCCESS;
            }
            break;
#endif
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Engine::App::Iterate() {
    return SDL_APP_CONTINUE;
}

void Engine::App::Quit(SDL_AppResult result) {
    // Handle resource cleanup here
}
