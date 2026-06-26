#include "Engine/Layers/scene_manager.h"

void Engine::SceneManager::PushScene(IScene *scene) {
    nextSceneAction = SceneAction(SceneAction::PUSH, scene);
}

void Engine::SceneManager::PopScene() {
    nextSceneAction = SceneAction(SceneAction::POP);
}

void Engine::SceneManager::ReplaceScene(IScene *scene) {
    nextSceneAction = SceneAction(SceneAction::REPLACE, scene);
}

void Engine::SceneManager::OnUpdate(float dt) {
    if (sceneStack.empty() && nextSceneAction.type != SceneAction::PUSH) return;

    if (nextSceneAction.type != SceneAction::NONE) {
        ApplySceneAction();
    }

    CurrentScene().Update(dt);
}

void Engine::SceneManager::OnRender() {
    if (sceneStack.empty()) return;

    CurrentScene().Render();
}

void Engine::SceneManager::OnInput(AppEvent event) const {
    if (sceneStack.empty()) return;

    CurrentScene().OnInput(event);
}

void Engine::SceneManager::Cleanup() {
}

void Engine::SceneManager::ApplySceneAction() {
    IScene *deleteScene = nullptr;

    switch (nextSceneAction.type) {
        case SceneAction::REPLACE:
            if (nextSceneAction.scene == nullptr) break;
            if (!sceneStack.empty()) {
                deleteScene = sceneStack.back();
                sceneStack.pop_back();
            }
            sceneStack.emplace_back(nextSceneAction.scene);
            break;
        case SceneAction::PUSH:
            if (nextSceneAction.scene == nullptr) break;
            sceneStack.emplace_back(nextSceneAction.scene);
            break;
        case SceneAction::POP:
            if (sceneStack.size() <= 1) break; // Don't pop last scene
            deleteScene = sceneStack.back();
            sceneStack.pop_back();
            break;
        default: break; // No action
    }

    // Invalidate the action after applying
    nextSceneAction.type = SceneAction::NONE;

    if (deleteScene != nullptr) {
        deleteScene->OnDestroy();
        delete deleteScene;
        deleteScene = nullptr;
    }
}
