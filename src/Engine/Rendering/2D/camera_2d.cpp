#include "Engine/Rendering/2D/camera_2d.h"

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Core/defines.h"

namespace Engine {
  Camera2D::Camera2D() : Camera2D(LOGICAL_APP_WIDTH, LOGICAL_APP_HEIGHT) {
  }

  Camera2D::Camera2D(int width, int height, int startX, int startY) : Camera2D(
    static_cast<float>(width), static_cast<float>(height), static_cast<float>(startX), static_cast<float>(startY)) {
  }

  Camera2D::Camera2D(float width, float height, float startX, float startY) : width(width), height(height) {
    cameraPosition.x = startX;
    cameraPosition.y = startY;
    projection = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
  }

  glm::mat4 Camera2D::GetViewMatrix() const {
    // Center around which we want to rotate the camera
    auto center = glm::vec3(width * 0.5f, height * 0.5f, 0);

    auto view = glm::mat4(1.0f);
    view = glm::translate(view, center);
    view = glm::rotate(view, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::scale(view, glm::vec3(zoomFactor, zoomFactor, 1.0f));
    view = glm::translate(view, -center - cameraPosition);

    return projection * view;
  }

  void Camera2D::Translate(float x, float y) {
    cameraPosition.x += x;
    cameraPosition.y += y;
  }

  void Camera2D::Rotate(float radians) {
    rotation += radians;
  }

  void Camera2D::Zoom(float amount) {
    zoomFactor += amount;
  }

  void Camera2D::Reset() {
    cameraPosition = glm::vec3(0.0f);
    zoomFactor = 1.0f;
    rotation = 0.0f;
  }

  Vec2f Camera2D::ScreenToWorld(
    Vec2f position, SDL_Window *window, const glm::mat4 &viewMatrix) const {
    if (window) {
      position = GetPresentationModeParams(window, position);
    }

    // Convert screen coordinates to Normalized Device Coordinates (NDC)
    float ndcX = (position.x / width) * 2.0f - 1.0f;
    float ndcY = 1.0f - (position.y / height) * 2.0f;
    glm::vec4 worldPos =
        glm::inverse(viewMatrix) * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);

    // Divide by w component just in case the projection is not orthogonal
    if (worldPos.w != 0.0f) {
      worldPos.x /= worldPos.w;
      worldPos.y /= worldPos.w;
    }

    return Vec2f{worldPos.x, worldPos.y};
  }

  Vec2f Camera2D::ScreenToWorld(
    Vec2f position, const glm::mat4 &viewMatrix) const {
    return ScreenToWorld(position, nullptr, viewMatrix);
  }

  Vec2f Camera2D::ScreenToWorld(Vec2f position, SDL_Window *window) const {
    return ScreenToWorld(position, window, GetViewMatrix());
  }

  Vec2f Camera2D::WorldToScreen(Vec2f position, const glm::mat4 &viewMatrix) const {
    // Convert world coordinates to Normalized Device Coordinates (NDC)
    glm::vec4 ndc = viewMatrix * glm::vec4(position.x, position.y, 0.0f, 1.0f);

    // Perspective division
    if (ndc.w != 0.0f) {
      ndc.x /= ndc.w;
      ndc.y /= ndc.w;
    }

    // Convert NDC to screen coordinates
    float screenX = (ndc.x + 1.0f) * 0.5f * width;
    float screenY = (1.0f - ndc.y) * 0.5f * height;

    return {screenX, screenY};
  }

  Vec2f Camera2D::WorldToScreen(Vec2f position) const {
    return WorldToScreen(position, GetViewMatrix());
  }

  Rectf Camera2D::GetCameraWorldBounds() const {
    auto viewMatrix = GetViewMatrix();
    Vec2f corners[4] = {
      ScreenToWorld(Vec2f{0.0f, 0.0f}, viewMatrix),
      ScreenToWorld(Vec2f{width, 0.0f}, viewMatrix),
      ScreenToWorld(Vec2f{0.0f, height}, viewMatrix),
      ScreenToWorld(Vec2f{width, height}, viewMatrix)
    };

    float minX = corners[0].x, maxX = corners[0].x;
    float minY = corners[0].y, maxY = corners[0].y;

    for (int i = 1; i < 4; i++) {
      minX = std::min(minX, corners[i].x);
      maxX = std::max(maxX, corners[i].x);
      minY = std::min(minY, corners[i].y);
      maxY = std::max(maxY, corners[i].y);
    }

    return {minX, minY, maxX - minX, maxY - minY};
  }

  Vec2f Camera2D::GetPresentationModeParams(SDL_Window *window, Vec2f &position) const {
    int winW = 0, winH = 0;
    SDL_GetWindowSize(window, &winW, &winH);

    if (winW > 0 && winH > 0) {
      auto windowSize = Vec2f(winW, winH);
      auto targetSize = Vec2f(width, height);

      auto mode = App::GetLayer<AppContext>().logicalPresentation;
      auto rect = GetNormalizedWindowRectSize(mode, windowSize, targetSize);

      // Map the window coordinate to normalized viewport coordinates [0.0, 1.0]
      float normX = (position.x - (rect[0] * windowSize.x)) / (rect[2] * windowSize.x);
      float normY = (position.y - (rect[1] * windowSize.y)) / (rect[3] * windowSize.y);

      // Set the mapped coordinates in target screen space
      position.x = normX * width;
      position.y = normY * height;
    }

    return {position};
  }
}
