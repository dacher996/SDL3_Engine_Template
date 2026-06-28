#include "Engine/Layers/texture_manager.h"

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Core/logger.h"
#include "Engine/Core/texture_sampler_type.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3_image/SDL_image.h"
#include "Engine/Layers/texture_region_manager.h"

namespace Engine {
  bool TextureManager::RegisterTexture(Uint16 id, Texture *texture) {
    if (texture == nullptr)
      return false;
    if (m_textures.contains(id)) {
      LOG_WARN(std::format("Texture slot {} already taken.", id));
      return false;
    }
    m_textures[id] = texture;
    return true;
  }

  Texture *TextureManager::GetTexture(const Uint16 id) {
    if (m_textures.contains(id)) {
      return m_textures[id];
    }
    return nullptr;
  }

  void TextureManager::UnloadTexture(Uint16 id) {
    if (!m_textures.contains(id))
      return;
    App::GetLayer<TextureRegionManager>().ClearRegionsForTexture(id);
    m_textures[id]->Free(App::GetLayer<AppContext>().gpuDevice);
    m_textures.erase(id);
  }

  void TextureManager::Cleanup() {
    auto &ctx = App::GetLayer<AppContext>();
    for (const auto texture: m_textures | std::views::values) {
      texture->Free(ctx.gpuDevice);
    }

    m_textures.clear();
  }

  SDL_Surface *TextureManager::ImageToSurface(const char *imageFilename,
                                              int desiredChannels) {
    const std::string fullPath = std::format(
      "{}assets/{}", App::GetLayer<AppContext>().basePath, imageFilename);
    SDL_PixelFormat format;
    SDL_Surface *result = IMG_Load(fullPath.c_str());

    if (!result) {
      ENGINE_LOG_SDL_ERROR("Couldn't load image.");
      return nullptr;
    }

    if (desiredChannels == 4) {
      format = SDL_PIXELFORMAT_ARGB8888;
    } else {
      // Unsupported pixel format
      SDL_DestroySurface(result);
      ENGINE_LOG_ERROR("Unsupported number of channels.");
      return nullptr;
    }

    if (result->format != format) {
      SDL_Surface *next = SDL_ConvertSurface(result, format);
      SDL_DestroySurface(result);
      result = next;
    }

    return result;
  }

  SDL_Surface *
  TextureManager::ImageToSurfaceFromMemory(std::vector<Uint8> &pixelData,
                                           int width, int height) {
    if (width < 0 || height < 0)
      return nullptr;

    auto result = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_ARGB8888,
                                        pixelData.data(), width * 4);
    if (!result) {
      ENGINE_LOG_SDL_ERROR("Couldn't create surface from pixel data.");
      return nullptr;
    }

    return result;
  }

  Texture *
  TextureManager::UploadTextures(const std::vector<std::string> &files) {
    if (files.empty())
      return nullptr;

    // Get the dimensions of the first layer
    auto surface = ImageToSurface(files.at(0).c_str(), 4);
    int width = surface->w;
    int height = surface->h;
    size_t rowSizeInBytes = width * 4;

    std::vector<Uint8> pixelData;
    pixelData.resize(rowSizeInBytes * height * files.size());
    int i = 0;
    auto destIterator = pixelData.begin();

    while (surface != nullptr) {
      auto rawPixelsSpan = std::span(static_cast<const Uint8 *>(surface->pixels),
                                     surface->pitch * surface->h);
      std::ranges::copy(rawPixelsSpan, destIterator);
      SDL_DestroySurface(surface);

      destIterator += rowSizeInBytes;
      ++i;

      if (i >= files.size())
        break;
      surface = ImageToSurface(files.at(i).c_str(), 4);
    }

    return UploadTextures(pixelData, width, height);
  }

  Texture *
  TextureManager::UploadTextures(const std::vector<Uint8> &texturePixels,
                                 int width, int height) {
    auto layerCount =
        static_cast<int>(texturePixels.size() / (width * height * 4));
    if (layerCount < 1 || width < 1 || height < 1)
      return nullptr;

    auto ctx = App::GetLayer<AppContext>();
    auto data = new Texture{
      .width = static_cast<Uint32>(width),
      .height = static_cast<Uint32>(height),
      .layers = layerCount,
      .samplerHint = POINT_CLAMP,
    };
    // Anon function for easy data cleanup
    auto freeData = [&data, &ctx]() {
      data->Free(ctx.gpuDevice);
      delete data;
    };
    // Prepare GPU texture and its transfer buffer
    auto textureBufferDeleter = [&](SDL_GPUTransferBuffer *buffer) {
      if (!buffer)
        SDL_ReleaseGPUTransferBuffer(ctx.gpuDevice, buffer);
    };
    std::unique_ptr<SDL_GPUTransferBuffer, decltype(textureBufferDeleter)>
        textureTransferBuffer{
          // Use a custom deleter to free up the resource whenever we exit the
          // current scope
          nullptr,
          textureBufferDeleter,
        };

    SDL_GPUTextureCreateInfo textureCreateInfo{
      .type = SDL_GPU_TEXTURETYPE_2D_ARRAY,
      .format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
      .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
      .width = data->width,
      .height = data->height,
      .layer_count_or_depth = static_cast<Uint32>(layerCount),
      .num_levels = 1,
    };
    data->texture = SDL_CreateGPUTexture(ctx.gpuDevice, &textureCreateInfo);
    if (!data->texture) {
      ENGINE_LOG_SDL_ERROR("Unable to create GPU texture.");
      freeData();
      return nullptr;
    }

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo{
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
      .size = data->GetTextureSize() * layerCount,
    };
    textureTransferBuffer.reset(
      SDL_CreateGPUTransferBuffer(ctx.gpuDevice, &transferBufferCreateInfo));
    if (!textureTransferBuffer) {
      ENGINE_LOG_SDL_ERROR("Unable to create transfer buffer.");
      freeData();
      return nullptr;
    }

    // Copy surface data
    {
      auto textureDataPtr{
        static_cast<Uint8 *>(SDL_MapGPUTransferBuffer(
          ctx.gpuDevice, textureTransferBuffer.get(), false))
      };
      if (!textureDataPtr) {
        ENGINE_LOG_SDL_ERROR("Unable to acquire texture data pointer.");
        freeData();
        return nullptr;
      }

      // Copy image data to transfer buffer
      std::span textureDataSpan{textureDataPtr, data->GetTextureSize()};
      std::ranges::copy(texturePixels, textureDataSpan.begin());

      // Unmap and free unused resources
      SDL_UnmapGPUTransferBuffer(ctx.gpuDevice, textureTransferBuffer.get());
    }

    // Do a copy pass
    {
      auto commandBuffer = SDL_AcquireGPUCommandBuffer(ctx.gpuDevice);
      auto copyPass = SDL_BeginGPUCopyPass(commandBuffer);
      if (!copyPass) {
        ENGINE_LOG_SDL_ERROR("Unable to begin copy pass.");
        freeData();
        return nullptr;
      }

      // Upload texture(s)
      Uint32 i = 0;
      while (i < layerCount) {
        SDL_GPUTextureTransferInfo textureTransferInfo{
          .transfer_buffer = textureTransferBuffer.get(),
          .offset = i * data->GetTextureSize(),
        };
        SDL_GPUTextureRegion destination{
          .texture = data->texture,
          .layer = i,
          .w = data->width,
          .h = data->height,
          .d = 1,
        };
        SDL_UploadToGPUTexture(copyPass, &textureTransferInfo, &destination,
                               false);
        ++i;
      }

      SDL_EndGPUCopyPass(copyPass);

      if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
        ENGINE_LOG_SDL_ERROR("Unable to submit GPU command buffer.");
        freeData();
        return nullptr;
      }
    }

    return data;
  }

  Texture *TextureManager::CreateRenderTarget(Uint32 width, Uint32 height) {
    SDL_GPUTextureCreateInfo textureCreateInfo{
      .type = SDL_GPU_TEXTURETYPE_2D,
      .format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
      .usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
      .width = width,
      .height = height,
      .layer_count_or_depth = 1,
      .num_levels = 1,
    };
    auto texture =
        SDL_CreateGPUTexture(App::GetLayer<AppContext>().gpuDevice, &textureCreateInfo);
    if (!texture) {
      ENGINE_LOG_SDL_ERROR("Unable to create render target.");
      return nullptr;
    }

    return new Texture{
      .texture = texture,
      .width = width,
      .height = height,
      .layers = 1,
    };
  }
}
