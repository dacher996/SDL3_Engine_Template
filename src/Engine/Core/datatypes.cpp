#include "Engine/Core/datatypes.h"

namespace Engine {
    const Vec2i Vec2i::ZERO = Vec2i(0);
    const Vec2i Vec2i::ONE = Vec2i(1);

    const Vec2f Vec2f::ZERO = Vec2f(0.0f);
    const Vec2f Vec2f::ONE = Vec2f(1.0f);

    const Vec3i Vec3i::ZERO = Vec3i(0);
    const Vec3i Vec3i::ONE = Vec3i(1);

    const Vec3f Vec3f::ZERO = Vec3f(0.0f);
    const Vec3f Vec3f::ONE = Vec3f(1.0f);

    const Recti Recti::ZERO = Recti(0, 0, 0, 0);
    const Rectf Rectf::ZERO = Rectf(0, 0, 0, 0);

    Vec2i::operator SDL_Point() const { return SDL_Point(x, y); }
    Vec2f::operator SDL_FPoint() const { return SDL_FPoint(x, y); }
    Recti::operator SDL_Rect() const { return SDL_Rect({x, y, w, h}); }
    Rectf::operator SDL_FRect() const { return SDL_FRect({x, y, w, h}); }
}
