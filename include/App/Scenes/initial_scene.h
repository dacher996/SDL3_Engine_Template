#ifndef APP_INITIAL_SCENE_H
#define APP_INITIAL_SCENE_H

#include "Engine/Core/scene_interface.h"

namespace YourProject {
    class InitialScene : public Engine::IScene {
    public:
        void Update(float dt) override;

        void Render() override;

        void OnEvent(Engine::AppEvent event) override;

        void OnDestroy() override;

    private:
        void SetDefaults();

        bool LoadShaders();

        bool LoadAtlases();

        bool m_ranOnce{false};
        Uint16 m_defaultTextureId{0};
        Uint16 m_defaultGraphicsPipeline{0};
        Uint16 m_defaultPixelRegion{0};
    };
}

#endif //APP_INITIAL_SCENE_H
