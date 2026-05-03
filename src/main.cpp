#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include "Engine/app.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    auto app = new Engine::App(argc, argv);
    *appstate = app;
    return app->Init();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto app = static_cast<Engine::App *>(appstate);
    return app->Event(event);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto app = static_cast<Engine::App *>(appstate);
    return app->Iterate();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    auto app = static_cast<Engine::App *>(appstate);
    app->Quit(result);
    delete app;
}
