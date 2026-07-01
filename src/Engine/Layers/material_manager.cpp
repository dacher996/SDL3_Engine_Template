#include "Engine/Layers/material_manager.h"
#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Layers/texture_region_manager.h"
#include <ranges>

namespace Engine {
    Uint16 MaterialManager::CreateMaterial(Uint16 shaderId, Uint16 textureId) {
        Uint16 newId = m_nextId++;
        auto material = std::make_unique<Material>();
        material->id = newId;
        material->shaderId = shaderId;
        material->textureId = textureId;
        m_materials[newId] = std::move(material);
        return newId;
    }

    Uint16 MaterialManager::CreateMaterialFromRegion(Uint16 shaderId, Uint32 textureRegionId) {
        auto region = App::GetLayer<TextureRegionManager>().GetRegion(textureRegionId);
        return CreateMaterial(shaderId, region->textureId);
    }

    Material *MaterialManager::GetMaterial(Uint16 id) {
        auto it = m_materials.find(id);
        if (it != m_materials.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void MaterialManager::UnloadMaterial(Uint16 id) {
        auto it = m_materials.find(id);
        if (it != m_materials.end()) {
            auto device = App::GetLayer<AppContext>().gpuDevice;
            if (it->second->uniformBuffer.Valid()) {
                it->second->uniformBuffer.Free(device);
            }
            m_materials.erase(it);
        }
    }

    void MaterialManager::Cleanup() {
        auto device = App::GetLayer<AppContext>().gpuDevice;
        for (auto &material: m_materials | std::views::values) {
            if (material->uniformBuffer.Valid()) {
                material->uniformBuffer.Free(device);
            }
        }
        m_materials.clear();
    }

    void MaterialManager::SetDefaultMaterial(Uint16 id) {
        m_defaultMaterialId = id;
    }

    Material *MaterialManager::GetDefaultMaterial() {
        return GetMaterial(m_defaultMaterialId);
    }

    const std::unordered_map<Uint16, std::unique_ptr<Material> > &MaterialManager::GetMaterials() const {
        return m_materials;
    }
}
