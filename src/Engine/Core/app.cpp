#include "Engine/Core/app.h"
#include "Engine/Core/logger.h"
#include "Engine/Core/app_context.h"

#include <cassert>

#include "Engine/Core/defines.h"
#include "Engine/Layers/graphics_pipeline_manager.h"
#include "Engine/Layers/scene_manager.h"
#include "Engine/Layers/texture_manager.h"
#include "Engine/Layers/texture_region_manager.h"
#include "Engine/Layers/texture_sampler_manager.h"
#include "Engine/Rendering/2D/renderer_2d.h"

static Engine::App *s_appHandle = nullptr;

Engine::App::App(int argc, char *argv[]) : m_window{nullptr, &SDL_DestroyWindow},
                                           m_gpuDevice{nullptr, &SDL_DestroyGPUDevice},
                                           m_lastFrameTime{} {
    s_appHandle = this;
    Logger::Init();

    AddLayer(AppContext{
        .basePath = SDL_GetBasePath(),
        .logicalWidth = LOGICAL_APP_WIDTH,
        .logicalHeight = LOGICAL_APP_HEIGHT,
        .windowWidth = INITIAL_WINDOW_WIDTH,
        .windowHeight = INITIAL_WINDOW_HEIGHT,
        .logicalPresentation = LogicalPresentation::Overscan,
    });
}

SDL_AppResult Engine::App::Init() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    auto &appContext = GetLayer<AppContext>();

    m_gpuDevice.reset(SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr));
    m_window.reset(SDL_CreateWindow(APP_NAME, appContext.windowWidth, appContext.windowHeight,
                                    SDL_WINDOW_RESIZABLE));

    if (m_gpuDevice == nullptr || m_window == nullptr ||
        !SDL_ClaimWindowForGPUDevice(m_gpuDevice.get(), m_window.get())) {
        ENGINE_LOG_SDL_ERROR("Window creation failed");
        return SDL_APP_FAILURE;
    }

    appContext.gpuDevice = m_gpuDevice.get();
    appContext.window = m_window.get();

    // Setup present mode of the gpu swapchain
    {
        SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
        if (SDL_WindowSupportsGPUPresentMode(m_gpuDevice.get(), m_window.get(), SDL_GPU_PRESENTMODE_IMMEDIATE)) {
            presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
        } else if (SDL_WindowSupportsGPUPresentMode(m_gpuDevice.get(), m_window.get(), SDL_GPU_PRESENTMODE_MAILBOX)) {
            presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
        }
        SDL_SetGPUSwapchainParameters(m_gpuDevice.get(), m_window.get(), SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);
    }

    // Init Core systems
    {
        AddLayer<GraphicsPipelineManager>();
        AddLayer<TextureSamplerManager>();
        AddLayer<TextureManager>();
        AddLayer<TextureRegionManager>();
        AddLayer<SceneManager>();
        AddLayer<Renderer2D>();
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Engine::App::Event(SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (SDL_GetWindowID(m_window.get()) == event->window.windowID) {
                return SDL_APP_SUCCESS;
            }
        case SDL_EVENT_WINDOW_RESIZED: {
            auto &appContext = GetLayer<AppContext>();
            appContext.windowWidth = event->window.data1;
            appContext.windowHeight = event->window.data2;
            GetLayer<SceneManager>().OnEvent(
                static_cast<AppEvent>(WindowResizedEvent(event->window.data1, event->window.data2))
            );
            return SDL_APP_CONTINUE;
        }
        break;
#ifdef DEBUG_MODE
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_ESCAPE) {
                return SDL_APP_SUCCESS;
            }
            break;
#endif
        default:
            break;
    }

    GetLayer<SceneManager>().OnEvent(static_cast<AppEvent>(SDLEvent(event)));

    return SDL_APP_CONTINUE;
}

SDL_AppResult Engine::App::Iterate() {
    // Calculate delta time
    {
        float newTime = SDL_GetTicks() / 1000.0f;
        GetLayer<AppContext>().deltaTime = newTime - m_lastFrameTime;
        m_lastFrameTime = newTime;
    }

    auto &sceneManager = GetLayer<SceneManager>();
    sceneManager.OnUpdate(GetLayer<AppContext>().deltaTime);
    sceneManager.OnRender();

    return SDL_APP_CONTINUE;
}

void Engine::App::Quit(SDL_AppResult result) {
    // Manually cleanup registered systems
    GetLayer<SceneManager>().Cleanup();
    GetLayer<GraphicsPipelineManager>().Cleanup();
    GetLayer<TextureSamplerManager>().Cleanup();
    GetLayer<TextureManager>().Cleanup();

    if (!m_gpuDevice && !m_window) SDL_ReleaseWindowFromGPUDevice(m_gpuDevice.get(), m_window.get());

    GetLayer<AppContext>().gpuDevice = nullptr;
}

Engine::App &Engine::App::Get() {
    assert(s_appHandle);
    return *s_appHandle;
}
