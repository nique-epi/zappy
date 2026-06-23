/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** IncantationRenderer
*/

#include "Render/Entity/IncantationRenderer.hpp"
#include <raylib.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <vector>
#include "Render/Entity/IncantationRendererConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "Render/WindowConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

std::vector<Texture2D> frames;
Model planeModel{};

void drawSprite(const Texture2D& tex, float worldX, float worldZ,
                unsigned char alpha) {
  const float scale = cfg::TILE_SIZE * cfg::INCANTATION_CIRCLE_SCALE;
  planeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;
  const Vector3 pos{worldX, cfg::INCANTATION_GROUND_Y, worldZ};
  BeginBlendMode(BLEND_ALPHA);
  DrawModelEx(planeModel, pos, {0.0F, 1.0F, 0.0F},
              cfg::INCANTATION_ROTATION_DEG, {scale, scale, scale},
              {255, 255, 255, alpha});
  EndBlendMode();
}

}  // namespace

void IncantationRenderer::loadTextures() {
  frames.reserve(cfg::INCANTATION_FRAME_COUNT);
  for (int i = 0; i < cfg::INCANTATION_FRAME_COUNT; ++i) {
    char path[256];
    std::snprintf(path, sizeof(path), ASSETS_DIR "/incantation/001_FX_%04d.png",
                  i);
    Texture2D tex = LoadTexture(path);
    if (tex.id == 0) {
      std::printf("[IncantationRenderer] ERROR: failed to load %s\n", path);
    }
    frames.push_back(tex);
  }
  planeModel = LoadModelFromMesh(GenMeshPlane(1.0F, 1.0F, 1, 1));
  planeModel.materials[0].shader =
      LoadShader(nullptr, ASSETS_DIR "/shaders/alpha_discard.frag");
}

void IncantationRenderer::unloadTextures() {
  planeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = {};
  for (auto& tex : frames) {
    UnloadTexture(tex);
  }
  frames.clear();
  UnloadModel(planeModel);
}

void IncantationRenderer::draw3D(WorldState& world) {
  if (frames.empty()) {
    return;
  }
  const int frameIndex =
      static_cast<int>(GetTime() * cfg::INCANTATION_ANIM_FPS) %
      static_cast<int>(frames.size());
  const Texture2D& tex = frames[frameIndex];

  for (const auto& incantation : world.activeIncantations) {
    drawSprite(tex, tileToWorld(incantation.x), tileToWorld(incantation.y),
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
    drawSprite(tex, tileToWorld(end.x), tileToWorld(end.y), alpha);
  }

  std::erase_if(world.finishedIncantations, [&now](const IncantationEnd& end) {
    return std::chrono::duration<float>(now - end.endTime).count() >=
           cfg::INCANTATION_FLASH_DURATION;
  });
}

}  // namespace zappy::gui
