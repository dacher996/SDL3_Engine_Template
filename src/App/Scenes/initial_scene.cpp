#include "App/Scenes/initial_scene.h"

#include "Engine/Core/app.h"
#include "Engine/Core/logger.h"
#include "Engine/Layers/graphics_pipeline_manager.h"
#include "Engine/Layers/scene_manager.h"
#include "Engine/Layers/texture_sampler_manager.h"
#include "Engine/Rendering/graphics_pipeline_creation_info.h"

namespace YourProject {
    void InitialScene::Update(float dt) {
        if (m_ranOnce) return;
        m_ranOnce = true;

        // Load necessary data or assets here
        // Load level data, initialize game state, etc.
        // Populate level store or other layers as needed

        LoadShaders();
        Engine::App::GetLayer<Engine::TextureSamplerManager>().GetSampler(Engine::POINT_CLAMP);

        // Go to next scene after everything has finished loading
        // Engine::App::GetLayer<Engine::SceneManager>().ReplaceScene(new MyScene());
    }

    void InitialScene::Render() {
    }

    void InitialScene::OnInput(Engine::AppEvent event) {
    }

    void InitialScene::OnDestroy() {
    }

    void InitialScene::LoadShaders() {
        using namespace Engine;

        // Load necessary shaders
        auto vertShader{GraphicsPipelineManager::LoadShader("Default.vert", 0, 1, 2, 0)};
        auto fragShader{GraphicsPipelineManager::LoadShader("Default.frag", 1, 0, 0, 0)};
        auto screenVertShader{GraphicsPipelineManager::LoadShader("TextureToScreen.vert", 0, 1, 0, 0)};
        auto screenFragShader{GraphicsPipelineManager::LoadShader("TextureToScreen.frag", 1, 0, 0, 0)};


        auto defaultPipeline = GraphicsPipelineManager::CreateGraphicsPipeline(
            vertShader, fragShader, GraphicsPipelineCreationInfo{});
        if (!defaultPipeline) {
            ENGINE_LOG_SDL_ERROR("Unable to create graphics pipeline: default.");
            return;
        }

        auto textureToScreenPipeline = GraphicsPipelineManager::CreateGraphicsPipeline(
            screenVertShader, screenFragShader, GraphicsPipelineCreationInfo{});
        if (!textureToScreenPipeline) {
            ENGINE_LOG_SDL_ERROR("Unable to create graphics pipeline: textureToScreen.");
            return;
        }

        // Register pipelines
        auto &pipelineManager = App::GetLayer<GraphicsPipelineManager>();
        pipelineManager.RegisterPipeline(0, defaultPipeline);
        pipelineManager.RegisterPipeline(1, textureToScreenPipeline);


        // Free up unused shaders
        GraphicsPipelineManager::UnloadShader(vertShader);
        GraphicsPipelineManager::UnloadShader(fragShader);
        GraphicsPipelineManager::UnloadShader(screenVertShader);
        GraphicsPipelineManager::UnloadShader(screenFragShader);
    }
}

