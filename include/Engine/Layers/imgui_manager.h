#ifndef ENGINE_IMGUI_MANAGER_H
#define ENGINE_IMGUI_MANAGER_H

#include "Engine/Core/layer_interface.h"
#include "SDL3/SDL.h"

namespace Engine {
    class ImGuiManager : ILayer {
    public:
        ImGuiManager();

        /// Initializes the system
        void Init();

        /// Performs a cleanup before being removed
        void Cleanup();

        /// Handles events. Returns value depending on whether the input should be forwarded to the app (false) or
        /// consumed by the ImGUI system (true)
        bool HandleEvent(SDL_Event *event);

        /// Initialized the environment for a new frame
        void StartFrame();

        /// Prepares the data for frame rendering
        void Render();

    private:
        float m_scaleFactor{1.0f};
        bool m_calledRender{false};


        void ConfigureStyle();
    };
}

#endif //ENGINE_IMGUI_MANAGER_H
