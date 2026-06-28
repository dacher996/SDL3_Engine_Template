#ifndef APP_INHOUSE_SPRITESHEET_PARSER_H
#define APP_INHOUSE_SPRITESHEET_PARSER_H

#include "Engine/Core/texture_region_parser.h"
#include <string>
#include <vector>
#include <utility>
#include "SDL3/SDL_stdinc.h"

namespace YourProject {
    /// Parses an inhouse JSON format to extract texture regions.
    class InhouseSpritesheetParser : public Engine::TextureRegionParser {
    public:
        /// Constructor
        /// \param jsonPath Relative path to the JSON file (will be appended to SDL_GetBasePath())
        /// \param textureId The ID of the texture this spritesheet belongs to
        InhouseSpritesheetParser(std::string jsonPath, Uint16 textureId);

        ~InhouseSpritesheetParser() override = default;

        /// Parses the file and returns the regions
        std::vector<std::pair<Uint32, Engine::TextureRegion> > Parse() override;

    private:
        std::string m_jsonPath;
        Uint16 m_textureId;
    };
} // namespace Engine

#endif // APP_INHOUSE_SPRITESHEET_PARSER_H
