#include "Engine/app.h"
#include "Engine/logger.h"
#include <assert.h>

static Engine::App *s_appHandle = nullptr;

Engine::App::App(int argc, char *argv[]) : m_window{nullptr, &SDL_DestroyWindow} {
    s_appHandle = this;
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
    float newTime = SDL_GetTicks() / 1000.0f;
    float deltaTime = newTime - m_lastFrameTime;
    m_lastFrameTime = newTime;

    return SDL_APP_CONTINUE;
}

void Engine::App::Quit(SDL_AppResult result) {
    // Handle resource cleanup here
}

Engine::App &Engine::App::Get() {
    assert(s_appHandle);
    return *s_appHandle;
}
