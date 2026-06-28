#ifndef ENGINE_DATATYPES_H
#define ENGINE_DATATYPES_H

#include "SDL3/SDL_rect.h"

namespace Engine {
    template<typename T>
    struct Vec2 {
        T x, y;

        Vec2(T val) : x(val), y(val) {
        }

        Vec2(T x, T y) : x(x), y(y) {
        }

        Vec2(const Vec2 &v) = default;

        Vec2(Vec2 &&v) noexcept = default;

        Vec2 &operator=(const Vec2 &v) = default;

        Vec2 &operator=(Vec2 &&v) noexcept = default;

        bool operator==(const Vec2 &v) const { return (x == v.x) && (y == v.y); }

        bool operator!=(const Vec2 &v) const { return !(*this == v); }
    };

    template<typename T>
    struct Vec3 {
        T x, y, z;

        Vec3(T val) : x(val), y(val), z(val) {
        }

        Vec3(T x, T y, T z) : x(x), y(y), z(z) {
        }

        Vec3(const Vec3 &v) = default;

        Vec3(Vec3 &&v) noexcept = default;

        Vec3 &operator=(const Vec3 &v) = default;

        Vec3 &operator=(Vec3 &&v) noexcept = default;

        bool operator==(const Vec3 &v) const { return x == v.x && y == v.y && z == v.z; }

        bool operator!=(const Vec3 &v) const { return !(*this == v); }
    };

    template<typename T>
    struct Rect {
        T x, y, w, h;

        Rect(Vec2<T> position, Vec2<T> size) : x(position.x), y(position.y), w(size.x), h(size.y) {
        }

        Rect(T x, T y, T w, T h) : x(x), y(y), w(w), h(h) {
        }

        Rect(const Rect &v) = default;

        Rect(Rect &&v) noexcept = default;

        Rect &operator=(const Rect &v) = default;

        Rect &operator=(Rect &&v) noexcept = default;

        bool operator==(const Rect &v) const { return x == v.x && y == v.y && w == v.w && h == v.h; }

        bool operator!=(const Rect &v) const { return !(*this == v); }
    };
}


namespace Engine {
    struct Vec2i : Vec2<int> {
        using Vec2<int>::Vec2;

        operator SDL_Point() const;

        static const Vec2i ZERO;
        static const Vec2i ONE;
    };

    struct Vec2f : Vec2<float> {
        using Vec2<float>::Vec2;

        operator SDL_FPoint() const;

        static const Vec2f ZERO;
        static const Vec2f ONE;
    };

    struct Vec3i : Vec3<int> {
        using Vec3<int>::Vec3;

        static const Vec3i ZERO;
        static const Vec3i ONE;
    };

    struct Vec3f : Vec3<float> {
        using Vec3<float>::Vec3;

        static const Vec3f ZERO;
        static const Vec3f ONE;
    };

    struct Recti : Rect<int> {
        using Rect<int>::Rect;

        operator SDL_Rect() const;

        static const Recti ZERO;
    };

    struct Rectf : Rect<float> {
        using Rect<float>::Rect;

        operator SDL_FRect() const;

        static const Rectf ZERO;
    };
}

#endif //ENGINE_DATATYPES_H
