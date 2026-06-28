#include "Engine/Core/color.h"

#include <string>

namespace Engine {
    Color::operator Uint32() const {
        return (static_cast<uint32_t>(a) << 24)
               | (static_cast<uint32_t>(b) << 16)
               | (static_cast<uint32_t>(g) << 8)
               | static_cast<uint32_t>(r);
    }

    Color::operator SDL_Color() const { return SDL_Color{r, g, b, a}; }

    std::array<Uint8, 4> Color::uint32ToUint8ArrayBE(Uint32 value) {
        return {
            static_cast<uint8_t>((value >> 24) & 0xFF), // a
            static_cast<uint8_t>((value >> 16) & 0xFF), // b
            static_cast<uint8_t>((value >> 8) & 0xFF), // g
            static_cast<uint8_t>(value & 0xFF) // r
        };
    }

    std::array<Uint8, 4> Color::uint32ToUint8ArrayLE(Uint32 value) {
        return {
            static_cast<uint8_t>(value & 0xFF), // r
            static_cast<uint8_t>((value >> 8) & 0xFF), // g
            static_cast<uint8_t>((value >> 16) & 0xFF), // b
            static_cast<uint8_t>((value >> 24) & 0xFF) // a
        };
    }

    Color Color::fromHex(const std::string &hex) {
        std::string cleanHex = hex;
        if (cleanHex.size() == 7 && cleanHex[0] == '#') {
            cleanHex = cleanHex.substr(1);
        }

        const int len = static_cast<int>(cleanHex.size());
        if (len != 6 && len != 8) {
            throw std::invalid_argument("Invalid hex color: " + hex);
        }

        Color color;
        color.r = static_cast<uint8_t>(std::stoi(cleanHex.substr(0, 2), nullptr, 16));
        color.g = static_cast<uint8_t>(std::stoi(cleanHex.substr(2, 2), nullptr, 16));
        color.b = static_cast<uint8_t>(std::stoi(cleanHex.substr(4, 2), nullptr, 16));
        if (len == 8) {
            color.a = static_cast<uint8_t>(std::stoi(cleanHex.substr(6, 2), nullptr, 16));
        }

        return color;
    }

    Color Color::withAlpha(const Color &color, Uint8 alpha) {
        return Color(color.r, color.g, color.b, alpha);
    }

    // Colors taken from: https://www.rapidtables.com/web/color/RGB_Color.html
    const Color Color::ALICE_BLUE = Color{240, 248, 255, 255};
    const Color Color::ANTIQUE_WHITE = Color{250, 235, 215, 255};
    const Color Color::AQUA = Color{0, 255, 255, 255};
    const Color Color::AQUA_MARINE = Color{127, 255, 212, 255};
    const Color Color::AZURE = Color{240, 255, 255, 255};
    const Color Color::BEIGE = Color{245, 245, 220, 255};
    const Color Color::BISQUE = Color{255, 228, 196, 255};
    const Color Color::BLACK = Color{0, 0, 0, 255};
    const Color Color::BLANCHED_ALMOND = Color{255, 235, 205, 255};
    const Color Color::BLUE = Color{0, 0, 255, 255};
    const Color Color::BLUE_VIOLET = Color{138, 43, 226, 255};
    const Color Color::BROWN = Color{165, 42, 42, 255};
    const Color Color::BURLY_WOOD = Color{222, 184, 135, 255};
    const Color Color::CADET_BLUE = Color{95, 158, 160, 255};
    const Color Color::CHARTREUSE = Color{127, 255, 0, 255};
    const Color Color::CHOCOLATE = Color{210, 105, 30, 255};
    const Color Color::CORAL = Color{255, 127, 80, 255};
    const Color Color::CORN_FLOWER_BLUE = Color{100, 149, 237, 255};
    const Color Color::CORN_SILK = Color{255, 248, 220, 255};
    const Color Color::CRIMSON = Color{220, 20, 60, 255};
    const Color Color::CYAN = Color{0, 255, 255, 255};
    const Color Color::DARK_BLUE = Color{0, 0, 139, 255};
    const Color Color::DARK_CYAN = Color{0, 139, 139, 255};
    const Color Color::DARK_GOLDEN_ROD = Color{184, 134, 11, 255};
    const Color Color::DARK_GRAY = Color{169, 169, 169, 255};
    const Color Color::DARK_GREEN = Color{0, 100, 0, 255};
    const Color Color::DARK_KHAKI = Color{189, 183, 107, 255};
    const Color Color::DARK_MAGENTA = Color{139, 0, 139, 255};
    const Color Color::DARK_OLIVE_GREEN = Color{85, 107, 47, 255};
    const Color Color::DARK_ORANGE = Color{255, 140, 0, 255};
    const Color Color::DARK_ORCHID = Color{153, 50, 204, 255};
    const Color Color::DARK_RED = Color{139, 0, 0, 255};
    const Color Color::DARK_SALMON = Color{233, 150, 122, 255};
    const Color Color::DARK_SEA_GREEN = Color{143, 188, 143, 255};
    const Color Color::DARK_SLATE_BLUE = Color{72, 61, 139, 255};
    const Color Color::DARK_SLATE_GRAY = Color{47, 79, 79, 255};
    const Color Color::DARK_TURQUOISE = Color{0, 206, 209, 255};
    const Color Color::DARK_VIOLET = Color{148, 0, 211, 255};
    const Color Color::DEEP_PINK = Color{255, 20, 147, 255};
    const Color Color::DEEP_SKY_BLUE = Color{0, 191, 255, 255};
    const Color Color::DIM_GRAY = Color{105, 105, 105, 255};
    const Color Color::DODGER_BLUE = Color{30, 144, 255, 255};
    const Color Color::FIREBRICK = Color{178, 34, 34, 255};
    const Color Color::FLORAL_WHITE = Color{255, 250, 240, 255};
    const Color Color::FOREST_GREEN = Color{34, 139, 34, 255};
    const Color Color::GAINSBORO = Color{220, 220, 220, 255};
    const Color Color::GHOST_WHITE = Color{248, 248, 255, 255};
    const Color Color::GOLD = Color{255, 215, 0, 255};
    const Color Color::GOLDEN_ROD = Color{218, 165, 32, 255};
    const Color Color::GRAY = Color{128, 128, 128, 255};
    const Color Color::GREEN = Color{0, 128, 0, 255};
    const Color Color::GREEN_YELLOW = Color{173, 255, 47, 255};
    const Color Color::HONEYDEW = Color{240, 255, 240, 255};
    const Color Color::HOT_PINK = Color{255, 105, 180, 255};
    const Color Color::INDIAN_RED = Color{205, 92, 92, 255};
    const Color Color::INDIGO = Color{75, 0, 130, 255};
    const Color Color::IVORY = Color{255, 255, 240, 255};
    const Color Color::KHAKI = Color{240, 230, 140, 255};
    const Color Color::LAVENDER = Color{230, 230, 250, 255};
    const Color Color::LAVENDER_BLUSH = Color{255, 240, 245, 255};
    const Color Color::LAWN_GREEN = Color{124, 252, 0, 255};
    const Color Color::LEMON_CHIFFON = Color{255, 250, 205, 255};
    const Color Color::LIGHT_BLUE = Color{173, 216, 230, 255};
    const Color Color::LIGHT_CORAL = Color{240, 128, 128, 255};
    const Color Color::LIGHT_CYAN = Color{224, 255, 255, 255};
    const Color Color::LIGHT_GOLDEN_ROD_YELLOW = Color{250, 250, 210, 255};
    const Color Color::LIGHT_GRAY = Color{211, 211, 211, 255};
    const Color Color::LIGHT_GREEN = Color{144, 238, 144, 255};
    const Color Color::LIGHT_PINK = Color{255, 182, 193, 255};
    const Color Color::LIGHT_SALMON = Color{255, 160, 122, 255};
    const Color Color::LIGHT_SEA_GREEN = Color{32, 178, 170, 255};
    const Color Color::LIGHT_SKY_BLUE = Color{135, 206, 250, 255};
    const Color Color::LIGHT_SLATE_GRAY = Color{119, 136, 153, 255};
    const Color Color::LIGHT_STEEL_BLUE = Color{176, 196, 222, 255};
    const Color Color::LIGHT_YELLOW = Color{255, 255, 224, 255};
    const Color Color::LIME = Color{0, 255, 0, 255};
    const Color Color::LIME_GREEN = Color{50, 205, 50, 255};
    const Color Color::LINEN = Color{250, 240, 230, 255};
    const Color Color::MAGENTA = Color{255, 0, 255, 255};
    const Color Color::MAROON = Color{128, 0, 0, 255};
    const Color Color::MEDIUM_AQUA_MARINE = Color{102, 205, 170, 255};
    const Color Color::MEDIUM_BLUE = Color{0, 0, 205, 255};
    const Color Color::MEDIUM_ORCHID = Color{186, 85, 211, 255};
    const Color Color::MEDIUM_PURPLE = Color{147, 112, 219, 255};
    const Color Color::MEDIUM_SEA_GREEN = Color{60, 179, 113, 255};
    const Color Color::MEDIUM_SLATE_BLUE = Color{123, 104, 238, 255};
    const Color Color::MEDIUM_SPRING_GREEN = Color{0, 250, 154, 255};
    const Color Color::MEDIUM_TURQUOISE = Color{72, 209, 204, 255};
    const Color Color::MEDIUM_VIOLET_RED = Color{199, 21, 133, 255};
    const Color Color::MIDNIGHT_BLUE = Color{25, 25, 112, 255};
    const Color Color::MINT_CREAM = Color{245, 255, 250, 255};
    const Color Color::MISTY_ROSE = Color{255, 228, 225, 255};
    const Color Color::MOCCASIN = Color{255, 228, 181, 255};
    const Color Color::NAVAJO_WHITE = Color{255, 222, 173, 255};
    const Color Color::NAVY = Color{0, 0, 128, 255};
    const Color Color::OLD_LACE = Color{253, 245, 230, 255};
    const Color Color::OLIVE = Color{128, 128, 0, 255};
    const Color Color::OLIVE_DRAB = Color{107, 142, 35, 255};
    const Color Color::ORANGE = Color{255, 165, 0, 255};
    const Color Color::ORANGE_RED = Color{255, 69, 0, 255};
    const Color Color::ORCHID = Color{218, 112, 214, 255};
    const Color Color::PALE_GOLDEN_ROD = Color{238, 232, 170, 255};
    const Color Color::PALE_GREEN = Color{152, 251, 152, 255};
    const Color Color::PALE_TURQUOISE = Color{175, 238, 238, 255};
    const Color Color::PALE_VIOLET_RED = Color{219, 112, 147, 255};
    const Color Color::PAPAYA_WHIP = Color{255, 239, 213, 255};
    const Color Color::PEACH_PUFF = Color{255, 218, 185, 255};
    const Color Color::PERU = Color{205, 133, 63, 255};
    const Color Color::PINK = Color{255, 192, 203, 255};
    const Color Color::PLUM = Color{221, 160, 221, 255};
    const Color Color::POWDER_BLUE = Color{176, 224, 230, 255};
    const Color Color::PURPLE = Color{128, 0, 128, 255};
    const Color Color::RED = Color{255, 0, 0, 255};
    const Color Color::ROSY_BROWN = Color{188, 143, 143, 255};
    const Color Color::ROYAL_BLUE = Color{65, 105, 225, 255};
    const Color Color::SADDLE_BROWN = Color{139, 69, 19, 255};
    const Color Color::SALMON = Color{250, 128, 114, 255};
    const Color Color::SANDY_BROWN = Color{244, 164, 96, 255};
    const Color Color::SEA_GREEN = Color{46, 139, 87, 255};
    const Color Color::SEA_SHELL = Color{255, 245, 238, 255};
    const Color Color::SIENNA = Color{160, 82, 45, 255};
    const Color Color::SILVER = Color{192, 192, 192, 255};
    const Color Color::SKY_BLUE = Color{135, 206, 235, 255};
    const Color Color::SLATE_BLUE = Color{106, 90, 205, 255};
    const Color Color::SLATE_GRAY = Color{112, 128, 144, 255};
    const Color Color::SNOW = Color{255, 250, 250, 255};
    const Color Color::SPRING_GREEN = Color{0, 255, 127, 255};
    const Color Color::STEEL_BLUE = Color{70, 130, 180, 255};
    const Color Color::TAN = Color{210, 180, 140, 255};
    const Color Color::TEAL = Color{0, 128, 128, 255};
    const Color Color::THISTLE = Color{216, 191, 216, 255};
    const Color Color::TOMATO = Color{255, 99, 71, 255};
    const Color Color::TURQUOISE = Color{64, 224, 208, 255};
    const Color Color::VIOLET = Color{238, 130, 238, 255};
    const Color Color::WHEAT = Color{245, 222, 179, 255};
    const Color Color::WHITE = Color{255, 255, 255, 255};
    const Color Color::WHITE_SMOKE = Color{245, 245, 245, 255};
    const Color Color::YELLOW = Color{255, 255, 0, 255};
    const Color Color::YELLOW_GREEN = Color{154, 205, 50, 255};
}
