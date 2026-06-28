#ifndef ENGINE_CAMERA_2D_H
#define ENGINE_CAMERA_2D_H

#include "Engine/Core/datatypes.h"
#include "SDL3/SDL_video.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
    class Camera2D {
    public:
        Camera2D();

        Camera2D(int width, int height, int startX = 0, int startY = 0);

        Camera2D(float width, float height, float startX = 0, float startY = 0);

        [[nodiscard]] glm::mat4 GetViewMatrix() const;

        void Translate(float x, float y);

        void Rotate(float radians);

        void Zoom(float amount);

        void Reset();

        [[nodiscard]] Vec2f ScreenToWorld(Vec2f position, SDL_Window *window,
                                          const glm::mat4 &viewMatrix) const;

        [[nodiscard]] Vec2f ScreenToWorld(Vec2f position, const glm::mat4 &viewMatrix) const;

        [[nodiscard]] Vec2f ScreenToWorld(Vec2f position, SDL_Window *window = nullptr) const;

        [[nodiscard]] Vec2f WorldToScreen(Vec2f position, const glm::mat4 &viewMatrix) const;

        [[nodiscard]] Vec2f WorldToScreen(Vec2f position) const;

        /// Returns the zoom amount
        [[nodiscard]] float GetZoom() const { return zoomFactor; }

        /// Returns the rotation amount in radians
        [[nodiscard]] float GetRotation() const { return rotation; }

        /// Returns camera bounds expressed in world coordinates as (x,y,w,h)
        [[nodiscard]] Rectf GetCameraWorldBounds() const;

    private:
        float width, height;
        float rotation{0.0f};
        float zoomFactor{1.0f};
        glm::mat4 projection{};
        glm::vec3 cameraPosition{0.0f};

        Vec2f GetPresentationModeParams(SDL_Window *window, Vec2f &position) const;
    };
}

#endif //ENGINE_CAMERA_2D_H
