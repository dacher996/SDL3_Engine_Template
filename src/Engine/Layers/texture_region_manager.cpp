#include "Engine/Layers/texture_region_manager.h"

namespace Engine {

    void TextureRegionManager::AddRegion(Uint32 key, const TextureRegion& region) {
        m_regions[key] = region;
        m_textureToRegionKeys[region.textureId].push_back(key);
    }
    
    const TextureRegion* TextureRegionManager::GetRegion(Uint32 key) const {
        auto it = m_regions.find(key);
        if (it != m_regions.end()) {
            return &it->second;
        }
        return nullptr;
    }

    void TextureRegionManager::ImportTextureRegions(Uint16 textureId, TextureRegionParser& parser) {
        auto parsedRegions = parser.Parse();
        for (const auto& [key, region] : parsedRegions) {
            TextureRegion newRegion = region;
            newRegion.textureId = textureId; // Ensure textureId matches the one we are assigning to
            AddRegion(key, newRegion);
        }
    }

    void TextureRegionManager::ClearRegionsForTexture(Uint16 textureId) {
        auto it = m_textureToRegionKeys.find(textureId);
        if (it != m_textureToRegionKeys.end()) {
            for (Uint32 key : it->second) {
                m_regions.erase(key);
            }
            m_textureToRegionKeys.erase(it);
        }
    }

    void TextureRegionManager::SetPixelRegion(Uint32 regionId) {
        m_pixelRegionId = regionId;
    }

    const TextureRegion* TextureRegionManager::GetPixelRegion() const {
        return GetRegion(m_pixelRegionId);
    }

}
