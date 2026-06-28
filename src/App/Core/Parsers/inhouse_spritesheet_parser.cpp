#include "App/Core/Parsers/inhouse_spritesheet_parser.h"
#include "Engine/Core/logger.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <format>

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"

namespace YourProject {
    InhouseSpritesheetParser::InhouseSpritesheetParser(std::string jsonPath, Uint16 textureId)
        : m_jsonPath(std::move(jsonPath)), m_textureId(textureId) {
    }

    std::vector<std::pair<Uint32, Engine::TextureRegion> > InhouseSpritesheetParser::Parse() {
        auto &ctx = Engine::App::GetLayer<AppContext>();

        std::vector<std::pair<Uint32, Engine::TextureRegion> > regions;

        std::string fullPath = std::format("{}/assets/{}", ctx.basePath, m_jsonPath);

        std::ifstream file(fullPath);
        if (!file.is_open()) {
            LOG_ERROR("InhouseSpritesheetParser: Failed to open JSON file: {0}", fullPath);
            return regions;
        }

        nlohmann::json jsonData;
        try {
            file >> jsonData;
        } catch (const nlohmann::json::parse_error &e) {
            LOG_ERROR("InhouseSpritesheetParser: JSON Parse Error in {0}: {1}", fullPath, e.what());
            return regions;
        }

        if (!jsonData.contains("meta") || !jsonData["meta"].contains("w") || !jsonData["meta"].contains("h")) {
            LOG_ERROR("InhouseSpritesheetParser: Missing spritesheet metadata in {0}", fullPath);
            return regions;
        }

        float metaW = jsonData["meta"]["w"].get<float>();
        float metaH = jsonData["meta"]["h"].get<float>();

        if (metaW < 1.0f || metaH < 1.0f) {
            LOG_ERROR("InhouseSpritesheetParser: Invalid 'meta' width or height in {0}", fullPath);
            return regions;
        }

        if (jsonData.contains("regions") && jsonData["regions"].is_array()) {
            Engine::TextureRegion region{};

            for (const auto &regionJson: jsonData["regions"]) {
                if (!regionJson.contains("id") || !regionJson.contains("rect") || !regionJson.contains("layer")) {
                    LOG_WARN("InhouseSpritesheetParser: Skipping region: missing 'id', 'rect', or 'layer'");
                    continue;
                }

                Uint32 id = regionJson["id"].get<Uint32>();
                Uint8 layerId = regionJson["layer"].get<Uint8>();
                auto rectArray = regionJson["rect"];

                if (rectArray.size() != 4) {
                    LOG_WARN("InhouseSpritesheetParser: Region {0} has invalid 'rect' array size", id);
                    continue;
                }

                float x = rectArray[0].get<float>();
                float y = rectArray[1].get<float>();
                float w = rectArray[2].get<float>();
                float h = rectArray[3].get<float>();


                region.textureId = m_textureId;
                region.layerId = layerId;

                // Normalized coordinates
                region.x = x / metaW;
                region.y = y / metaH;
                region.w = w / metaW;
                region.h = h / metaH;

                regions.emplace_back(id, region);
            }
        } else {
            LOG_WARN("InhouseSpritesheetParser: No 'regions' data found in {0}", fullPath);
        }

        return regions;
    }
} // namespace Engine
