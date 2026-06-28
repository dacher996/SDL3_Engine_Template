#ifndef ENGINE_LOGICAL_PRESENTATION_H
#define ENGINE_LOGICAL_PRESENTATION_H

#include "Engine/Core/datatypes.h"

namespace Engine {
  enum class LogicalPresentation {
    Disabled,
    Stretch,
    Letterbox,
    Overscan,
    IntegerScale
  };

  /// Returns rectangle parameters representing the full window
  /// as {x,y,w,h} in normalized coordinates [0.0, 1.0]
  static std::array<float, 4>
  GetNormalizedWindowRectSize(LogicalPresentation mode, const Vec2f &windowSize, const Vec2f &targetSize) {
    float rx = 0.0f;
    float ry = 0.0f;
    float rw = 1.0f;
    float rh = 1.0f;

    float w = windowSize.x;
    float h = windowSize.y;
    float windowAspect = w / h;
    float targetAspect = targetSize.x / targetSize.y;

    if (mode == LogicalPresentation::Disabled) {
      // No logical scaling. The render target is centered 1:1.
      rw = targetSize.x / w;
      rh = targetSize.y / h;
      rx = (1.0f - rw) * 0.5f;
      ry = (1.0f - rh) * 0.5f;
    } else if (mode == LogicalPresentation::Stretch) {
      // The content is stretched to fill the entire output resolution.
      rx = 0.0f;
      ry = 0.0f;
      rw = 1.0f;
      rh = 1.0f;
    } else if (mode == LogicalPresentation::Letterbox) {
      // Scale to fit the largest possible dimension, keeping aspect ratio (letterboxing/pillarboxing).
      if (windowAspect > targetAspect) {
        rw = (h * targetAspect) / w;
        rh = 1.0f;
        rx = (1.0f - rw) * 0.5f;
        ry = 0.0f;
      } else {
        rw = 1.0f;
        rh = (w / targetAspect) / h;
        rx = 0.0f;
        ry = (1.0f - rh) * 0.5f;
      }
    } else if (mode == LogicalPresentation::Overscan) {
      // Scale to fit the smallest dimension (scale to fill), preserving aspect ratio and cropping excess.
      if (windowAspect > targetAspect) {
        rw = 1.0f;
        rh = windowAspect / targetAspect;
        rx = 0.0f;
        ry = (1.0f - rh) * 0.5f;
      } else {
        rw = targetAspect / windowAspect;
        rh = 1.0f;
        rx = (1.0f - rw) * 0.5f;
        ry = 0.0f;
      }
    } else if (mode == LogicalPresentation::IntegerScale) {
      // Scale by integer multiples to fit as closely as possible, centered.
      int scaleX = w / static_cast<int>(targetSize.x);
      int scaleY = h / static_cast<int>(targetSize.y);
      int scale = std::min(scaleX, scaleY);
      if (scale < 1) scale = 1;
      rw = (targetSize.x * static_cast<float>(scale)) / w;
      rh = (targetSize.y * static_cast<float>(scale)) / h;
      rx = (1.0f - rw) * 0.5f;
      ry = (1.0f - rh) * 0.5f;
    }

    return {rx, ry, rw, rh};
  }
}

#endif //ENGINE_LOGICAL_PRESENTATION_H
