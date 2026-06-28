#ifndef ENGINE_SCENE_INTERFACE_H
#define ENGINE_SCENE_INTERFACE_H

#include <entt/entity/registry.hpp>

#include "Engine/Core/app_event.h"

namespace Engine {
    using Entity = entt::entity;
    using Registry = entt::registry;

    class IScene {
    public:
        virtual ~IScene() = default;

        /// Called once per frame to update the scene logic.
        virtual void Update(float dt) = 0;

        /// Called once per frame to render the scene.
        virtual void Render() = 0;

        /// Called when there is a new input available
        virtual void OnInput(AppEvent event) = 0;

        /// Called just before the scene is destroyed. At this point the scene is not part of the scene stack anymore.
        virtual void OnDestroy() = 0;
    };
}

#endif //ENGINE_SCENE_INTERFACE_H
