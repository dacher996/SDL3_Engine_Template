#ifndef ENGINE_APP_H
#define ENGINE_APP_H

#include "SDL3/SDL.h"
#include "entt/entt.hpp"

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

        template<typename T>
        T &GetLayer() { return m_registry.ctx().get<T>(); }

        template<typename T>
        T &AddLayer(const T &layer) { return m_registry.ctx().emplace<T>(layer); }

        static App &Get();

    private:
        float m_lastFrameTime;
        entt::registry m_registry;
        std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_window;
    };
}

#endif //ENGINE_APP_H
