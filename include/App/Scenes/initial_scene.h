#ifndef APP_INITIAL_SCENE_H
#define APP_INITIAL_SCENE_H

#include "Engine/Core/scene_interface.h"

namespace YourProject {
    class InitialScene : public Engine::IScene {
    public:
        void Update(float dt) override;

        void Render() override;

        void OnInput(Engine::AppEvent event) override;

        void OnDestroy() override;

    private:
        void LoadShaders();

        bool m_ranOnce{false};
    };
}

#endif //APP_INITIAL_SCENE_H
