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
#include "Render/TileGridConfig.hpp"
#include "Render/WindowConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

struct DirectionVec {
  float x;
  float z;
};

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

float tileWorldX(int gridX) {
  return (static_cast<float>(gridX) + cfg::TILE_CENTER_OFFSET) * cfg::TILE_SIZE;
}

float tileWorldZ(int gridY) {
  return (static_cast<float>(gridY) + cfg::TILE_CENTER_OFFSET) * cfg::TILE_SIZE;
}

void drawPlayerBody(Vector3 position, Color color) {
  const Vector3 basePos{position.x, cfg::PLAYER_BASE_Y, position.z};
  const Vector3 topPos{position.x, cfg::PLAYER_BASE_Y + cfg::PLAYER_HEIGHT,
                       position.z};
  DrawCylinderEx(basePos, topPos, cfg::PLAYER_RADIUS, cfg::PLAYER_RADIUS,
                 cfg::PLAYER_CYLINDER_SIDES, color);
  DrawCylinderWiresEx(basePos, topPos, cfg::PLAYER_RADIUS, cfg::PLAYER_RADIUS,
                      cfg::PLAYER_CYLINDER_SIDES,
                      ColorBrightness(color, -cfg::PLAYER_WIRE_DARKNESS));
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

void PlayerRenderer::draw3D(const WorldState& world) {
  for (const auto& player : world.players) {
    if (!player.alive) {
      continue;
    }
    const Vector3 position{tileWorldX(player.x), 0.0F, tileWorldZ(player.y)};
    const Color color = resolveTeamColor(world, player.teamName);

    drawPlayerBody(position, color);
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
        tileWorldX(player.x),
        cfg::PLAYER_LABEL_Y,
        tileWorldZ(player.y),
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
             cfg::PLAYER_LABEL_COLOR);
  }
}

}  // namespace zappy::gui
