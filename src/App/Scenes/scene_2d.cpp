#include "App/Scenes/scene_2d.h"

#include "App/Core/components.h"
#include "Engine/Core/app.h"
#include "Engine/Core/components.h"
#include "Engine/Rendering/2D/renderer_2d.h"

namespace YourProject {
    Scene2D::Scene2D() : Scene2D(Engine::Camera2D()) {
    }

    Scene2D::Scene2D(const Engine::Camera2D &camera) : m_camera(camera) {
    }

    Scene2D::~Scene2D() = default;

    void Scene2D::Update(float dt) {
    }

    void Scene2D::Render() {
        auto &renderer = Engine::App::GetLayer<Engine::Renderer2D>();
        renderer.Submit(PrepareSprites());
        renderer.Render();
    }

    void Scene2D::OnInput(Engine::AppEvent event) {
    }

    void Scene2D::OnDestroy() {
    }

    Engine::Camera2D &Scene2D::GetCamera() {
        return m_camera;
    }

    std::vector<Engine::Entity> Scene2D::GetVisibleEntities() {
        return GetVisibleEntities(Engine::Rectf::ZERO);
    }

    std::vector<Engine::Entity> Scene2D::GetVisibleEntities(Engine::Rectf padding) {
        std::vector<entt::entity> ents;
        SDL_FRect result;

        auto camWorldRegion = GetCamera().GetCameraWorldBounds();
        auto camToWorld = SDL_FRect{
            camWorldRegion.x - padding.x, camWorldRegion.y - padding.y,
            camWorldRegion.w + padding.w, camWorldRegion.h + padding.h
        };

        // Ideally, we'd want only to retrieve objects that are in the region of the camera
        // rather than iterating over every entity. A good case would be to use a QuadTree or a hybrid approach.
        m_registry.view<Engine::Position, Engine::Size, Sprite>().each(
            [&](Engine::Entity entity, Engine::Position pos, Engine::Size size, Sprite _) {
                SDL_FRect rect{pos.x, pos.y, static_cast<float>(size.x), static_cast<float>(size.y)};
                if (SDL_GetRectIntersectionFloat(&rect, &camToWorld, &result)) {
                    ents.push_back(entity);
                }
            });

        return ents;
    }

    Engine::SpriteSubmission Scene2D::PrepareSprites() {
        return PrepareSprites(m_camera);
    }

    Engine::SpriteSubmission Scene2D::PrepareSprites(Engine::Camera2D &camera) {
        auto visible = GetVisibleEntities();

        Engine::SpriteSubmission submission;
        submission.entries.reserve(static_cast<size_t>(static_cast<float>(visible.size()) * 1.2f));
        submission.viewMatrix = camera.GetViewMatrix();

        for (auto entityId: visible) {
            const auto &pos = m_registry.get<Engine::Position>(entityId);
            const auto &size = m_registry.get<Engine::Size>(entityId);
            const auto &sprite = m_registry.get<Sprite>(entityId);

            Engine::SpriteEntry entry{};
            entry.data.x = pos.x;
            entry.data.y = pos.y;
            entry.data.w = static_cast<float>(size.x);
            entry.data.h = static_cast<float>(size.y);
            entry.data.r = sprite.color.r;
            entry.data.g = sprite.color.g;
            entry.data.b = sprite.color.b;
            entry.data.a = sprite.color.a;
            entry.data.rotation = sprite.rotation;
            entry.data.tex_u = sprite.textureData.x;
            entry.data.tex_v = sprite.textureData.y;
            entry.data.tex_w = sprite.textureData.w;
            entry.data.tex_h = sprite.textureData.h;
            entry.data.textureLayerId = static_cast<float>(sprite.textureData.layerId);
            entry.depth = static_cast<Uint16>(pos.z);

            // Entities with shader components produce one entry per shader
            if (const auto *shader = m_registry.try_get<Engine::Shader>(entityId);
                shader && !shader->shaders.empty()) {
                for (auto sid: shader->shaders) {
                    entry.shaderId = sid.shaderId;
                    submission.entries.push_back(entry);
                }
            } else {
                // If no shader component attached, use the default shader
                entry.shaderId = 0;
                submission.entries.push_back(entry);
            }
        }

        return submission;
    }
}
