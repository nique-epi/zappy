/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerRenderer
*/

#include "Render/PlayerRenderer.hpp"
#include <raylib.h>
#include <cstddef>
#include <format>
#include <string>
#include "Render/PlayerRendererConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "Render/WindowConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr int IDLE_ANIM_INDEX = 1;

DirectionVec orientationDirection(Orientation orientation) {
  switch (orientation) {
    case Orientation::North:
      return {.x = 0.0F, .z = -1.0F};
    case Orientation::East:
      return {.x = 1.0F, .z = 0.0F};
    case Orientation::South:
      return {.x = 0.0F, .z = 1.0F};
    case Orientation::West:
      return {.x = -1.0F, .z = 0.0F};
  }
  return {.x = 0.0F, .z = -1.0F};
}

Color resolveTeamColor(const WorldState& world, const std::string& teamName) {
  for (std::size_t i = 0; i < world.teams.size(); ++i) {
    if (world.teams[i] == teamName) {
      return cfg::TEAM_COLORS[i % cfg::TEAM_COLORS.size()];
    }
  }
  return cfg::TEAM_COLORS[0];
}

void drawOrientationArrow(Vector3 position, Orientation orientation) {
  const DirectionVec dir = orientationDirection(orientation);
  const float baseX = position.x + (dir.x * cfg::PLAYER_ARROW_OFFSET);
  const float baseZ = position.z + (dir.z * cfg::PLAYER_ARROW_OFFSET);

  const Vector3 coneBase{baseX, cfg::PLAYER_CENTER_Y, baseZ};
  const Vector3 coneTip{
      baseX + (dir.x * cfg::PLAYER_ARROW_HEIGHT),
      cfg::PLAYER_CENTER_Y,
      baseZ + (dir.z * cfg::PLAYER_ARROW_HEIGHT),
  };

  DrawCylinderEx(coneBase, coneTip, cfg::PLAYER_ARROW_RADIUS, 0.0F,
                 cfg::PLAYER_ARROW_SIDES, cfg::PLAYER_ARROW_COLOR);
}

}  // namespace

PlayerRenderer::PlayerRenderer() {
  model_ = LoadModel(cfg::PLAYER_MODEL_PATH);
  animations_ = LoadModelAnimations(cfg::PLAYER_MODEL_PATH, &animCount_);
}

PlayerRenderer::~PlayerRenderer() {
  if (animations_ != nullptr) {
    UnloadModelAnimations(animations_, animCount_);
  }
  UnloadModel(model_);
}

void PlayerRenderer::draw3D(const WorldState& world) {
  if (animCount_ > IDLE_ANIM_INDEX) {
    animFrame_ = (animFrame_ + 1) % animations_[IDLE_ANIM_INDEX].frameCount;
    UpdateModelAnimation(model_, animations_[IDLE_ANIM_INDEX], animFrame_);
  }

  for (const auto& player : world.players) {
    if (!player.alive) {
      continue;
    }
    const Vector3 position{tileToWorld(player.x), 0.0F, tileToWorld(player.y)};
    DrawModel(model_, position, cfg::PLAYER_MODEL_SCALE, WHITE);
    drawOrientationArrow(position, player.orientation);
  }
}

void PlayerRenderer::drawLevelLabels(const WorldState& world,
                                     const Camera3D& camera) {
  for (const auto& player : world.players) {
    if (!player.alive) {
      continue;
    }

    const Vector3 labelWorldPos{
        tileToWorld(player.x),
        cfg::PLAYER_LABEL_Y,
        tileToWorld(player.y),
    };

    const Vector2 screenPos = GetWorldToScreen(labelWorldPos, camera);
    const std::string label = std::format("Lv{}", player.level);
    const int textWidth =
        MeasureText(label.c_str(), cfg::PLAYER_LABEL_FONT_SIZE);
    const int drawX = static_cast<int>(screenPos.x) - (textWidth / 2);
    const int drawY = static_cast<int>(screenPos.y);

    DrawText(label.c_str(), drawX + cfg::PLAYER_LABEL_SHADOW_OFFSET,
             drawY + cfg::PLAYER_LABEL_SHADOW_OFFSET,
             cfg::PLAYER_LABEL_FONT_SIZE, cfg::PLAYER_LABEL_SHADOW_COLOR);
    DrawText(label.c_str(), drawX, drawY, cfg::PLAYER_LABEL_FONT_SIZE,
             resolveTeamColor(world, player.teamName));
  }
}

}  // namespace zappy::gui
