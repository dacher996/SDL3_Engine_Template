#include "Engine/Core/app.h"
#include "Engine/Core/logger.h"
#include "Engine/Core/app_context.h"

#include <assert.h>

#include "Engine/Layers/scene_manager.h"

static Engine::App *s_appHandle = nullptr;

Engine::App::App(int argc, char *argv[]) : m_window{nullptr, &SDL_DestroyWindow} {
    s_appHandle = this;
    Logger::Init();

    AddLayer(AppContext{
        .basePath = SDL_GetBasePath(),
        .gameWidth = 320,
        .gameHeight = 192,
        .windowWidth = 800,
        .windowHeight = 600,
    });
}

SDL_AppResult Engine::App::Init() {
    SDL_Init(SDL_INIT_VIDEO);

    auto &appContext = GetLayer<AppContext>();
    m_window.reset(SDL_CreateWindow("Project name", appContext.windowWidth, appContext.windowHeight,
                                    SDL_WINDOW_RESIZABLE));

    // Register layers
    {
        AddLayer(SceneManager());
    }

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
        case SDL_EVENT_WINDOW_RESIZED: {
            auto &appContext = GetLayer<AppContext>();
            appContext.windowWidth = event->window.data1;
            appContext.windowHeight = event->window.data2;
            GetLayer<SceneManager>().OnInput(
                static_cast<AppEvent>(WindowResizedEvent(event->window.data1, event->window.data2))
            );
            return SDL_APP_CONTINUE;
        }
        break;
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

    GetLayer<SceneManager>().OnInput(static_cast<AppEvent>(SDLEvent(event)));

    return SDL_APP_CONTINUE;
}

SDL_AppResult Engine::App::Iterate() {
    // Calculate delta time
    {
        float newTime = SDL_GetTicks() / 1000.0f;
        GetLayer<AppContext>().deltaTime = newTime - m_lastFrameTime;
        m_lastFrameTime = newTime;
    }

    auto &sceneManager = GetLayer<SceneManager>();
    sceneManager.OnUpdate(GetLayer<AppContext>().deltaTime);
    sceneManager.OnRender();

    return SDL_APP_CONTINUE;
}

void Engine::App::Quit(SDL_AppResult result) {
    // Handle resource cleanup here
}

Engine::App &Engine::App::Get() {
    assert(s_appHandle);
    return *s_appHandle;
}
