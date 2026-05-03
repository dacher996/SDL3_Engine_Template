#ifndef ENGINE_APP_H
#define ENGINE_APP_H

#include "SDL3/SDL.h"

#include <memory>

namespace Engine {
    class App {
    public:
        App(int argc, char *argv[]);

        virtual ~App() = default;

        virtual SDL_AppResult Init();

        virtual SDL_AppResult Event(SDL_Event *event);

        virtual SDL_AppResult Iterate();

        virtual void Quit(SDL_AppResult result);

    private:
        std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_window;
    };
}

#endif //ENGINE_APP_H
