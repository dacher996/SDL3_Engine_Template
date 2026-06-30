#ifndef ENGINE_TEXTURE_MANAGER_H
#define ENGINE_TEXTURE_MANAGER_H

#include <vector>
#include <unordered_map>

#include "Engine/Rendering/texture.h"
#include "SDL3/SDL_surface.h"

namespace Engine {
    /// Stores and manages the lifecycle of GPU Textures
    class TextureManager {
    public:
        /// Registers a GPU texture under the specific id.
        ///
        /// Returns success if the texture was registered. If the id was already taken, the texture won't be
        /// registered and the function will return with `false`.
        bool RegisterTexture(Uint16 id, Texture *texture);

        /// Retrieves a gpu texture previously registered under the provided id
        Texture *GetTexture(Uint16 id);

        /// Frees a GPU texture with a specific id from memory.
        void UnloadTexture(Uint16 id);

        /// Frees all pipelines from memory
        void Cleanup();

        /// Sets the default texture
        void SetDefaultTexture(Uint16 id);

        /// Retrieves the default texture
        Texture *GetDefaultTexture();

        /// Loads an image file into a surface.
        ///
        /// The surface needs to be freed manually once not in use.
        static SDL_Surface *ImageToSurface(const char *imageFilename, int desiredChannels = 4);

        /// Turns pixel data into a surface.
        ///
        /// The surface needs to be freed manually once not in use.
        static SDL_Surface *ImageToSurfaceFromMemory(std::vector<Uint8> &pixelData, int width, int height);

        /// Creates an array texture and pushes all data to the gpu.
        static Texture *UploadTextures(const std::vector<std::string> &files);

        /// Creates an array texture and pushes all data to the gpu.
        static Texture *UploadTextures(const std::vector<Uint8> &texturePixels, int width, int height);

        /// Creates a texture that can be used as a render target.
        ///
        /// The texture data needs to be cleared manually.
        static Texture *CreateRenderTarget(Uint32 width, Uint32 height);

    private:
        std::unordered_map<Uint16, Texture *> m_textures;
        Uint16 m_defaultTextureId{0};
    };
}

#endif //ENGINE_TEXTURE_MANAGER_H
