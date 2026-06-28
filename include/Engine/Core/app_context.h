#ifndef ENGINE_APP_CONTEXT_H
#define ENGINE_APP_CONTEXT_H

#include <string>
#include "SDL3/SDL_gpu.h"
#include "Engine/Rendering/logical_presentation.h"

/// Application context
struct AppContext {
    /// The working directory of the app
    std::string basePath;

    /// The delta time since last frame
    float deltaTime;

    /// The logical width of the app
    int logicalWidth;

    /// The logical height of the app
    int logicalHeight;

    /// The width of the window
    int windowWidth;

    /// The height of the window
    int windowHeight;

    /// GPU Device handle
    SDL_GPUDevice *gpuDevice;

    /// Window handle
    SDL_Window *window;

    /// The logical representation of the app
    Engine::LogicalPresentation logicalPresentation;
};

#endif //ENGINE_APP_CONTEXT_H
