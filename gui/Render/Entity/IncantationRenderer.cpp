/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** IncantationRenderer
*/

#include "Render/Entity/IncantationRenderer.hpp"
#include <raylib.h>
#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include "Render/Entity/IncantationRendererConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "Render/WindowConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr const char* FRAME_PATH_FORMAT =
    ASSETS_DIR "/incantation/001_FX_{:04d}.png";
constexpr const char* SHADER_PATH = ASSETS_DIR "/shaders/alpha_discard.frag";
constexpr const char* LOAD_ERROR =
    "[IncantationRenderer] ERROR: failed to load ";

void drawSprite(Model& planeModel, const Texture2D& tex, Vector2 worldPos,
                unsigned char alpha) {
  const float scale = cfg::TILE_SIZE * cfg::INCANTATION_CIRCLE_SCALE;
  planeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;
  const Vector3 pos{worldPos.x, cfg::INCANTATION_GROUND_Y, worldPos.y};
  auto tint = WHITE;
  tint.a = alpha;
  BeginBlendMode(BLEND_ALPHA);
  DrawModelEx(planeModel, pos, {0.0F, 1.0F, 0.0F},
              cfg::INCANTATION_ROTATION_DEG, {scale, scale, scale}, tint);
  EndBlendMode();
}

}  // namespace

void IncantationRenderer::loadAssets() {
  frames_.reserve(cfg::INCANTATION_FRAME_COUNT);
  for (int i = 0; i < cfg::INCANTATION_FRAME_COUNT; ++i) {
    const std::string path = std::format(FRAME_PATH_FORMAT, i);
    const Texture2D tex = LoadTexture(path.c_str());
    if (tex.id == 0) {
      std::cerr << LOAD_ERROR << path << '\n';
    }
    frames_.push_back(tex);
  }
  planeModel_ = LoadModelFromMesh(GenMeshPlane(1.0F, 1.0F, 1, 1));
  planeModel_.materials[0].shader = LoadShader(nullptr, SHADER_PATH);
}

void IncantationRenderer::draw(WorldState& world) {
  if (frames_.empty()) {
    return;
  }
  const int frameIndex =
      static_cast<int>(GetTime() * cfg::INCANTATION_ANIM_FPS) %
      static_cast<int>(frames_.size());
  const Texture2D& tex = frames_[frameIndex];

  for (const auto& incantation : world.activeIncantations) {
    drawSprite(planeModel_, tex,
               {tileToWorld(incantation.x), tileToWorld(incantation.y)},
               cfg::INCANTATION_ACTIVE_ALPHA);
  }

  const auto now = std::chrono::steady_clock::now();
  for (const auto& end : world.finishedIncantations) {
    const float elapsed =
        std::chrono::duration<float>(now - end.endTime).count();
    if (elapsed >= cfg::INCANTATION_FLASH_DURATION) {
      continue;
    }
    const auto alpha = static_cast<unsigned char>(
        static_cast<float>(cfg::INCANTATION_ACTIVE_ALPHA) *
        (1.0F - elapsed / cfg::INCANTATION_FLASH_DURATION));
    drawSprite(planeModel_, tex, {tileToWorld(end.x), tileToWorld(end.y)},
               alpha);
  }

  std::erase_if(world.finishedIncantations, [&now](const IncantationEnd& end) {
    return std::chrono::duration<float>(now - end.endTime).count() >=
           cfg::INCANTATION_FLASH_DURATION;
  });
}

void IncantationRenderer::drawOverlay([[maybe_unused]] WorldState& world,
                                      [[maybe_unused]] const Camera3D& camera) {
}

void IncantationRenderer::unloadAssets() {
  planeModel_.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = {};
  for (auto& tex : frames_) {
    UnloadTexture(tex);
  }
  frames_.clear();
  UnloadModel(planeModel_);
}

}  // namespace zappy::gui
