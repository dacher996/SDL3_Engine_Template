#ifndef ENGINE_MATERIAL_MANAGER_H
#define ENGINE_MATERIAL_MANAGER_H

#include "Engine/Core/layer_interface.h"
#include "Engine/Rendering/storage_buffer.h"
#include <memory>
#include <unordered_map>

namespace Engine {
  struct Material {
    Uint16 id;
    Uint16 shaderId;
    Uint16 textureId;
    StorageBuffer uniformBuffer;

    template<typename T>
    void SetUniformData(SDL_GPUDevice *device, const T &data) {
      if (!uniformBuffer.Valid() || uniformBuffer.size != sizeof(T)) {
        uniformBuffer.Resize<T>(device, 1);
      }

      auto *ptr =
          SDL_MapGPUTransferBuffer(device, uniformBuffer.transferBuffer, false);
      if (ptr) {
        std::memcpy(ptr, &data, sizeof(T));
        SDL_UnmapGPUTransferBuffer(device, uniformBuffer.transferBuffer);

        needsUpload = true;
      }
    }

  private:
    /// Internal state representing dirty flag for uniform buffer upload
    bool needsUpload{false};

    /// Allow access to the needsUpload flag (rather than exposing it publicly)
    friend class Renderer2D;
  };

  class MaterialManager : public ILayer {
  public:
    MaterialManager() = default;

    ~MaterialManager() = default;

    MaterialManager(MaterialManager &&) noexcept = default;

    MaterialManager &operator=(MaterialManager &&) noexcept = default;

    /// Creates a material from a graphics pipeline and a texture
    Uint16 CreateMaterial(Uint16 shaderId, Uint16 textureId);

    /// Creates a material from a graphics pipeline and a texture region
    Uint16 CreateMaterialFromRegion(Uint16 shaderId, Uint32 textureRegionId);

    /// Retrieves a material via an id
    Material *GetMaterial(Uint16 id);

    /// Unloads a material of given id
    void UnloadMaterial(Uint16 id);

    /// Clears all registered materials
    void Cleanup();

    /// Sets the default material
    void SetDefaultMaterial(Uint16 id);

    /// Retrieves the default material
    Material *GetDefaultMaterial();

    const std::unordered_map<Uint16, std::unique_ptr<Material> > &GetMaterials() const;

  private:
    std::unordered_map<Uint16, std::unique_ptr<Material> > m_materials;
    Uint16 m_nextId{0};
    Uint16 m_defaultMaterialId{0};
  };
} // namespace Engine

#endif // ENGINE_MATERIAL_MANAGER_H
