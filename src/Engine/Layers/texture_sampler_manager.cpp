#include "Engine/Layers/texture_sampler_manager.h"

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"

namespace Engine {
    void TextureSamplerManager::Cleanup() {
        const auto &ctx = App::GetLayer<AppContext>();
        for (int i = 0; i < m_samplers.size(); i++) {
            SDL_ReleaseGPUSampler(ctx.gpuDevice, m_samplers[i]);
            m_samplers[i] = nullptr;
        }
    }

    SDL_GPUSampler *TextureSamplerManager::GetSampler(TextureSamplerType type) {
        if (m_samplers[type] == nullptr) {
            auto createInfo = GetSamplerCreateInfo(type);
            m_samplers[type] = SDL_CreateGPUSampler(App::GetLayer<AppContext>().gpuDevice, &createInfo);
        }
        return m_samplers[type];
    }

    void TextureSamplerManager::UnloadSampler(TextureSamplerType type) {
        if (m_samplers[type] == nullptr) return;
        SDL_ReleaseGPUSampler(App::GetLayer<AppContext>().gpuDevice, m_samplers[type]);
        m_samplers[type] = nullptr;
    }

    SDL_GPUSamplerCreateInfo TextureSamplerManager::GetSamplerCreateInfo(TextureSamplerType samplerType) {
        switch (samplerType) {
            case LINEAR_WRAP: return SDL_GPUSamplerCreateInfo{
                    .min_filter = SDL_GPU_FILTER_LINEAR,
                    .mag_filter = SDL_GPU_FILTER_LINEAR,
                    .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
                    .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                    .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                    .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                };
            case LINEAR_CLAMP: return SDL_GPUSamplerCreateInfo{
                    .min_filter = SDL_GPU_FILTER_LINEAR,
                    .mag_filter = SDL_GPU_FILTER_LINEAR,
                    .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
                    .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                    .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                    .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                };
            case ANISOTROPIC_WRAP: return SDL_GPUSamplerCreateInfo{
                    .min_filter = SDL_GPU_FILTER_LINEAR,
                    .mag_filter = SDL_GPU_FILTER_LINEAR,
                    .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
                    .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                    .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                    .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                    .max_anisotropy = 4,
                    .enable_anisotropy = true,
                };
            case ANISOTROPIC_CLAMP: return SDL_GPUSamplerCreateInfo{
                    .min_filter = SDL_GPU_FILTER_LINEAR,
                    .mag_filter = SDL_GPU_FILTER_LINEAR,
                    .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
                    .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                    .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                    .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                    .max_anisotropy = 4,
                    .enable_anisotropy = true,
                };
            case POINT_WRAP: return SDL_GPUSamplerCreateInfo{
                    .min_filter = SDL_GPU_FILTER_NEAREST,
                    .mag_filter = SDL_GPU_FILTER_NEAREST,
                    .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
                    .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                    .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                    .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                };
            case POINT_CLAMP:
            default: return SDL_GPUSamplerCreateInfo{
                    .min_filter = SDL_GPU_FILTER_NEAREST,
                    .mag_filter = SDL_GPU_FILTER_NEAREST,
                    .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
                    .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                    .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                    .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                };
        }
    }
}
