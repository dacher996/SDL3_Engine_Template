#ifndef ENGINE_STORAGE_BUFFER_H
#define ENGINE_STORAGE_BUFFER_H
#include "Engine/Core/logger.h"
#include "SDL3/SDL_gpu.h"

namespace Engine {
    /// Convenience structure that unifies storage buffer data in one place
    struct StorageBuffer {
        SDL_GPUBuffer *buffer{nullptr};
        SDL_GPUTransferBuffer *transferBuffer{nullptr};
        Uint32 size{0};
        size_t itemCount{0};

        [[nodiscard]] bool Valid() const;

        void Free(SDL_GPUDevice *gpuDevice);

        template<typename T>
        void Resize(SDL_GPUDevice *gpuDevice, size_t newSize);
    };

    template<typename T>
    void StorageBuffer::Resize(SDL_GPUDevice *gpuDevice, size_t newSize) {
        if (newSize <= itemCount) return;

        // Free up existing resources
        if (Valid()) Free(gpuDevice);

        itemCount = newSize;
        size = static_cast<Uint32>(sizeof(T)) * newSize;

        // Create storage buffer
        SDL_GPUBufferCreateInfo spriteDataBufferCreateInfo{
            .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
            .size = size,
        };
        buffer = SDL_CreateGPUBuffer(gpuDevice, &spriteDataBufferCreateInfo);
        if (!buffer) {
            ENGINE_LOG_SDL_ERROR("Unable to create buffer for sprite data.");
            return;
        }

        // Create transfer buffer
        SDL_GPUTransferBufferCreateInfo spriteDataTransferBufferCreateInfo{
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = size,
        };
        transferBuffer = SDL_CreateGPUTransferBuffer(gpuDevice,
                                                     &spriteDataTransferBufferCreateInfo);
        if (!transferBuffer) {
            ENGINE_LOG_SDL_ERROR("Unable to create transfer buffer for sprite data.");
            return;
        }
    }
};

#endif //ENGINE_STORAGE_BUFFER_H
