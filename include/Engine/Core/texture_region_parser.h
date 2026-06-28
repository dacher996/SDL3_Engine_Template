#ifndef ENGINE_TEXTURE_REGION_PARSER_H
#define ENGINE_TEXTURE_REGION_PARSER_H

#include "Engine/Core/texture_region.h"
#include <vector>
#include <utility>
#include "SDL3/SDL_stdinc.h"

namespace Engine {
    /// Interface for parsing texture regions from a source
    class TextureRegionParser {
    public:
        virtual ~TextureRegionParser() = default;

        /// Parses the source and returns a vector of pairs containing the Uint32 key and the TextureRegion
        virtual std::vector<std::pair<Uint32, TextureRegion>> Parse() = 0;
    };
}

#endif //ENGINE_TEXTURE_REGION_PARSER_H
