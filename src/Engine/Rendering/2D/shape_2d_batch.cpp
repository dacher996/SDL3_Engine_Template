#include "Engine/Rendering/2D/shape_2d_batch.h"
#include "Engine/Core/app.h"
#include "Engine/Core/logger.h"
#include "Engine/Layers/texture_region_manager.h"
#include <cmath>

#include "Engine/Layers/material_manager.h"

namespace Engine {
  Shape2DBatch::Shape2DBatch(const glm::mat4 &viewMatrix) {
    m_submission.viewMatrix = viewMatrix;
  }

  void Shape2DBatch::DrawLine(float x0, float y0, float x1, float y1,
                              const Color &color, float thickness, Uint16 depth) {
    auto &texRegionManager = App::GetLayer<TextureRegionManager>();
    const TextureRegion *pixelRegion = texRegionManager.GetPixelRegion();
    if (!pixelRegion) {
      LOG_WARN("Shape2DBatch: Pixel region not set in TextureRegionManager. "
        "Cannot draw primitive.");
      return;
    }

    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = std::sqrt(dx * dx + dy * dy);
    float angle = std::atan2(dy, dx);

    // Calculate top-left position to center the line on the segment
    float sinA = std::sin(angle);
    float cosA = std::cos(angle);

    float px = x0 + (thickness / 2.0f) * sinA;
    float py = y0 - (thickness / 2.0f) * cosA;

    SpriteEntry entry{};
    entry.data.x = px;
    entry.data.y = py;
    entry.data.w = length;
    entry.data.h = thickness;
    entry.data.r = static_cast<float>(color.r);
    entry.data.g = static_cast<float>(color.g);
    entry.data.b = static_cast<float>(color.b);
    entry.data.a = static_cast<float>(color.a);
    entry.data.rotation = angle;
    entry.data.tex_x = pixelRegion->x;
    entry.data.tex_y = pixelRegion->y;
    entry.data.tex_w = pixelRegion->w;
    entry.data.tex_h = pixelRegion->h;
    entry.data.textureLayerId = static_cast<float>(pixelRegion->layerId);

    entry.depth = depth;
    entry.materialId = App::GetLayer<MaterialManager>().GetDefaultMaterial()->id;

    m_submission.entries.push_back(entry);
  }

  void Shape2DBatch::DrawLine(Vec2f pt0, Vec2f pt1, const Color &color, float thickness, Uint16 depth) {
    DrawLine(pt0.x, pt0.y, pt1.x, pt1.y, color, thickness, depth);
  }

  void Shape2DBatch::DrawRect(float x, float y, float w, float h,
                              const Color &color, float rotation, Uint16 depth) {
    auto &texRegionManager = App::GetLayer<TextureRegionManager>();
    const TextureRegion *pixelRegion = texRegionManager.GetPixelRegion();
    if (!pixelRegion) {
      LOG_WARN("Shape2DBatch: Pixel region not set in TextureRegionManager. "
        "Cannot draw primitive.");
      return;
    }

    DrawTexture(x, y, w, h, pixelRegion, color, rotation, depth);
  }

  void Shape2DBatch::DrawRect(Rectf rect, const Color &color, float rotation, Uint16 depth) {
    DrawRect(rect.x, rect.y, rect.w, rect.h, color, rotation, depth);
  }

  void Shape2DBatch::DrawTexture(float x, float y, float w, float h, const TextureRegion *region,
                                 const Color &color, float rotation, Uint16 depth) {
    if (!region) return;

    float px = x;
    float py = y;

    // If rotated, rotate around the center of the rectangle
    if (rotation != 0.0f) {
      float cx = x + w / 2.0f;
      float cy = y + h / 2.0f;

      float sinA = std::sin(rotation);
      float cosA = std::cos(rotation);

      // Center of sprite before rotation is (w/2, h/2)
      // We want Position + rotated(w/2, h/2) = (cx, cy)
      float rx = (w / 2.0f) * cosA - (h / 2.0f) * sinA;
      float ry = (w / 2.0f) * sinA + (h / 2.0f) * cosA;

      px = cx - rx;
      py = cy - ry;
    }

    SpriteEntry entry{};
    entry.data.x = px;
    entry.data.y = py;
    entry.data.w = w;
    entry.data.h = h;
    entry.data.r = static_cast<float>(color.r);
    entry.data.g = static_cast<float>(color.g);
    entry.data.b = static_cast<float>(color.b);
    entry.data.a = static_cast<float>(color.a);
    entry.data.rotation = rotation;
    entry.data.tex_x = region->x;
    entry.data.tex_y = region->y;
    entry.data.tex_w = region->w;
    entry.data.tex_h = region->h;
    entry.data.textureLayerId = static_cast<float>(region->layerId);

    entry.depth = depth;
    entry.materialId = App::GetLayer<MaterialManager>().GetDefaultMaterial()->id;;

    m_submission.entries.push_back(entry);
  }

  void Shape2DBatch::DrawTexture(Rectf rect, const TextureRegion *region,
                                 const Color &color, float rotation, Uint16 depth) {
    DrawTexture(rect.x, rect.y, rect.w, rect.h, region, color, rotation, depth);
  }

  void Shape2DBatch::DrawRectOutlined(float x, float y, float w, float h,
                                      const Color &color, float thickness,
                                      float rotation, Uint16 depth) {
    // We can draw 4 lines. To handle rotation properly, we calculate the 4
    // corners of the rotated rect and draw lines between them.

    float cx = x + w / 2.0f;
    float cy = y + h / 2.0f;

    float sinA = std::sin(rotation);
    float cosA = std::cos(rotation);

    // Helper to rotate a point around center
    auto rotatePoint = [&](float px, float py) {
      float dx = px - cx;
      float dy = py - cy;
      float rx = dx * cosA - dy * sinA;
      float ry = dx * sinA + dy * cosA;
      return glm::vec2(cx + rx, cy + ry);
    };

    glm::vec2 p0 = rotatePoint(x, y); // Top-left
    glm::vec2 p1 = rotatePoint(x + w, y); // Top-right
    glm::vec2 p2 = rotatePoint(x + w, y + h); // Bottom-right
    glm::vec2 p3 = rotatePoint(x, y + h); // Bottom-left

    DrawLine(p0.x, p0.y, p1.x, p1.y, color, thickness, depth);
    DrawLine(p1.x, p1.y, p2.x, p2.y, color, thickness, depth);
    DrawLine(p2.x, p2.y, p3.x, p3.y, color, thickness, depth);
    DrawLine(p3.x, p3.y, p0.x, p0.y, color, thickness, depth);
  }

  void Shape2DBatch::DrawRectOutlined(Rectf rect, const Color &color, float thickness, float rotation, Uint16 depth) {
    DrawRectOutlined(rect.x, rect.y, rect.w, rect.h, color, thickness, rotation, depth);
  }

  SpriteSubmission Shape2DBatch::End() { return std::move(m_submission); }
} // namespace Engine
