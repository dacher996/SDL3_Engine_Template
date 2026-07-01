#include "Engine/Layers/imgui_manager.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"

namespace Engine {
    ImGuiManager::ImGuiManager() {
        Init();
    }

    bool ImGuiManager::HandleEvent(SDL_Event *event) {
        ImGui_ImplSDL3_ProcessEvent(event);

        if (auto &io = ImGui::GetIO(); io.WantCaptureMouse || io.WantCaptureKeyboard) {
            return true;
        }

        return false;
    }

    void ImGuiManager::StartFrame() {
        // Start the Dear ImGui frame
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        m_calledRender = false;
    }

    void ImGuiManager::Render() {
        if (m_calledRender) return;
        ImGui::Render();
        m_calledRender = true;
    }

    void ImGuiManager::Init() {
        m_scaleFactor = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        auto &ctx = App::GetLayer<AppContext>();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

        ConfigureStyle();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForSDLGPU(ctx.window);
        ImGui_ImplSDLGPU3_InitInfo init_info = {};
        init_info.Device = ctx.gpuDevice;
        init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(ctx.gpuDevice, ctx.window);
        init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1; // Only used in multi-viewports mode.
        init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR; // Only used in multi-viewports mode.
        init_info.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
        ImGui_ImplSDLGPU3_Init(&init_info);
    }

    void ImGuiManager::Cleanup() {
        SDL_WaitForGPUIdle(App::GetLayer<AppContext>().gpuDevice);
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiManager::ConfigureStyle() {
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup scaling
        ImGuiStyle &style = ImGui::GetStyle();
        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.ScaleAllSizes(m_scaleFactor);

        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
        style.FontScaleDpi = m_scaleFactor;

        // Load Fonts
        // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
        //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
        // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //style.FontSizeBase = 20.0f;
        //io.Fonts->AddFontDefaultVector();
        //io.Fonts->AddFontDefaultBitmap();
        //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
        //IM_ASSERT(font != nullptr);
    }
}
