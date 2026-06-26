#ifndef ENGINE_SCENE_MANAGER_H
#define ENGINE_SCENE_MANAGER_H

#include <vector>

#include "Engine/Core/app_event.h"
#include "Engine/Core/layer_interface.h"
#include "Engine/Core/scene_interface.h"

namespace Engine {
    /// Manages the stack of scenes in the game.
    /// Handles pushing, popping, and replacing scenes.
    ///
    /// The GameContext manages the life-cycle of the scenes, ensuring proper cleanup when scenes are removed.
    class SceneManager : public ILayer {
    public:
        SceneManager() = default;

        /// Pushes a new scene onto the stack.
        void PushScene(IScene *scene);

        /// Pops the current scene.
        void PopScene();

        /// Replaces the current scene with the provided one. If there is no scene to replace, it just pushes the new scene.
        void ReplaceScene(IScene *scene);

        /// Returns a reference to the current scene. Assumes there is at least one scene in the stack.
        [[nodiscard]] IScene &CurrentScene() const { return *sceneStack.back(); }

        /// Processes the current scene.
        void OnUpdate(float dt);

        /// Processes the current scene.
        void OnRender();

        /// Forwards user input
        void OnInput(AppEvent event) const;

        /// Clears any managed resources
        void Cleanup();

    private:
        /// Represents a deferred scene action to be applied.
        struct SceneAction {
            enum ActionType { NONE, PUSH, POP, REPLACE } type;

            IScene *scene = nullptr; // Action performed on this scene (if applicable)

            SceneAction() : type(NONE) {
            }

            explicit SceneAction(const ActionType t, IScene *s = nullptr) : type(t), scene(s) {
            }

            SceneAction(const SceneAction &) = default;

            SceneAction &operator=(const SceneAction &) = default;

            SceneAction(SceneAction &&) noexcept = default;
        };

        std::vector<IScene *> sceneStack{};
        SceneAction nextSceneAction; // For deferred scene changes

        void ApplySceneAction();
    };
};

#endif //ENGINE_SCENE_MANAGER_H
