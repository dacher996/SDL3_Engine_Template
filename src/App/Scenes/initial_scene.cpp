#include "App/Scenes/initial_scene.h"

#include "App/Scenes/scene_2d.h"
#include "Engine/Core/app.h"
#include "Engine/Core/logger.h"
#include "App/Core/Parsers/inhouse_spritesheet_parser.h"
#include "Engine/Layers/graphics_pipeline_manager.h"
#include "Engine/Layers/scene_manager.h"
#include "Engine/Layers/texture_manager.h"
#include "Engine/Layers/texture_region_manager.h"
#include "Engine/Layers/texture_sampler_manager.h"
#include "Engine/Rendering/graphics_pipeline_creation_info.h"

using namespace Engine;

namespace YourProject {
    void InitialScene::Update(float dt) {
        if (m_ranOnce) return;
        m_ranOnce = true;

        // Populate level store or other layers as needed

        App::GetLayer<TextureSamplerManager>().GetSampler(POINT_CLAMP);
        if (!LoadShaders()) return;
        if (!LoadAtlases()) return;

        // Go to next scene after everything has finished loading
        App::GetLayer<SceneManager>().ReplaceScene(new Scene2D());
    }

    void InitialScene::Render() {
    }

    void InitialScene::OnEvent(AppEvent event) {
    }

    void InitialScene::OnDestroy() {
    }

    bool InitialScene::LoadShaders() {
        constexpr Uint16 defaultPipelineId = 0;
        constexpr Uint16 texToScreenPipelineId = 1;

        // Load necessary shaders
        auto vertShader{GraphicsPipelineManager::LoadShader("Default.vert", 0, 1, 2, 0)};
        auto fragShader{GraphicsPipelineManager::LoadShader("Default.frag", 1, 0, 0, 0)};
        auto screenVertShader{GraphicsPipelineManager::LoadShader("TextureToScreen.vert", 0, 1, 0, 0)};
        auto screenFragShader{GraphicsPipelineManager::LoadShader("TextureToScreen.frag", 1, 0, 0, 0)};


        auto defaultPipeline = GraphicsPipelineManager::CreateGraphicsPipeline(
            vertShader, fragShader, GraphicsPipelineCreationInfo{});
        if (!defaultPipeline) {
            ENGINE_LOG_SDL_ERROR("Unable to create graphics pipeline: default.");
            return false;
        }

        auto textureToScreenPipeline = GraphicsPipelineManager::CreateGraphicsPipeline(
            screenVertShader, screenFragShader, GraphicsPipelineCreationInfo{});
        if (!textureToScreenPipeline) {
            ENGINE_LOG_SDL_ERROR("Unable to create graphics pipeline: textureToScreen.");
            return false;
        }

        // Register pipelines
        auto &pipelineManager = App::GetLayer<GraphicsPipelineManager>();
        pipelineManager.RegisterPipeline(defaultPipelineId, defaultPipeline);
        pipelineManager.RegisterPipeline(texToScreenPipelineId, textureToScreenPipeline);


        // Free up unused shaders
        GraphicsPipelineManager::UnloadShader(vertShader);
        GraphicsPipelineManager::UnloadShader(fragShader);
        GraphicsPipelineManager::UnloadShader(screenVertShader);
        GraphicsPipelineManager::UnloadShader(screenFragShader);

        return true;
    }

    bool InitialScene::LoadAtlases() {
        // Load necessary data or assets here
        auto &textureManager = App::GetLayer<TextureManager>();

        constexpr Uint16 mainAtlasId = 0;

        std::vector<std::vector<std::string> > atlases{
            {"atlases/main_atlas.png"}
        };

        for (auto &atlas: atlases) {
            if (!App::GetLayer<TextureManager>().
                RegisterTexture(mainAtlasId, TextureManager::UploadTextures(atlas))) {
                ENGINE_LOG_ERROR("Error uploading texture data to GPU.");
                return false;
            }
        }

        InhouseSpritesheetParser parser("atlases/main_atlas.json", mainAtlasId);
        App::GetLayer<TextureRegionManager>().ImportTextureRegions(mainAtlasId, parser);

        return true;
    }
}

