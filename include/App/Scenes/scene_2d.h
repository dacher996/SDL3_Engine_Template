#ifndef APP_SCENE_2D_H
#define APP_SCENE_2D_H

#include "Engine/Core/scene_interface.h"
#include "Engine/Rendering/sprite_submission.h"
#include "Engine/Rendering/2D/camera_2d.h"

namespace YourProject {
    class Scene2D : public Engine::IScene {
    public:
        Scene2D();

        Scene2D(const Engine::Camera2D &camera);

        ~Scene2D() override;

        void Update(float dt) override;

        void Render() override;

        void OnEvent(Engine::AppEvent event) override;

        void OnDestroy() override;

    protected:
        virtual Engine::Camera2D &GetCamera();

        /// Returns all visible entities
        std::vector<Engine::Entity> GetVisibleEntities();

        virtual std::vector<Engine::Entity> GetVisibleEntities(Engine::Rectf padding);

        /// Build a SpriteSubmission from the current entity state. Use the main camera for object culling
        Engine::SpriteSubmission PrepareSprites();

        /// Build a SpriteSubmission from the current entity state. Use the provided camera for object culling.
        virtual Engine::SpriteSubmission PrepareSprites(Engine::Camera2D &camera);

        /// The active camera
        Engine::Camera2D m_camera;

        /// The scene registry where entities are stored
        Engine::Registry m_registry;
    };
}

#endif //APP_SCENE_2D_H
