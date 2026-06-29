#ifndef ENGINE_TEXTURE_REGION_MANAGER_H
#define ENGINE_TEXTURE_REGION_MANAGER_H

#include "Engine/Core/texture_region.h"
#include "Engine/Core/texture_region_parser.h"
#include <unordered_map>
#include <vector>

namespace Engine {
    /// Stores and manages the lifecycle of TextureRegions
    class TextureRegionManager {
    public:
        /// Manually add a region
        void AddRegion(Uint32 key, const TextureRegion &region);
        
        /// Retrieve a region by key (returns const pointer to avoid copying, nullptr if not found)
        const TextureRegion *GetRegion(Uint32 key) const;

        /// Import regions using a parser, associating them with a textureId
        void ImportTextureRegions(Uint16 textureId, TextureRegionParser &parser);

        /// Clears all regions associated with a specific texture ID
        void ClearRegionsForTexture(Uint16 textureId);

        /// Sets the region ID to be used as a solid white pixel for primitive rendering
        void SetPixelRegion(Uint32 regionId);

        /// Gets the region used for solid white pixel rendering
        const TextureRegion *GetPixelRegion() const;

    private:
        std::unordered_map<Uint32, TextureRegion> m_regions;
        std::unordered_map<Uint16, std::vector<Uint32> > m_textureToRegionKeys;
        Uint32 m_pixelRegionId{0};
    };
}

#endif //ENGINE_TEXTURE_REGION_MANAGER_H
