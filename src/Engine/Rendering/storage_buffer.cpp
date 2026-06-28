#include "Engine/Rendering/storage_buffer.h"

bool Engine::StorageBuffer::Valid() const {
    return buffer != nullptr && transferBuffer != nullptr;
}

void Engine::StorageBuffer::Free(SDL_GPUDevice *gpuDevice) {
    size = 0;
    if (transferBuffer) {
        SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
        transferBuffer = nullptr;
    }
    if (buffer) {
        SDL_ReleaseGPUBuffer(gpuDevice, buffer);
        buffer = nullptr;
    }
}
