/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerRenderer
*/

#include "Render/Entity/PlayerRenderer.hpp"
#include <raylib.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <format>
#include <numbers>
#include <string>
#include "Render/Entity/PlayerRendererConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

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

void drawPlayerBody(Vector3 position, Color color, float scale = 1.0F) {
  const float radius = cfg::PLAYER_RADIUS * scale;
  const float height = cfg::PLAYER_HEIGHT * scale;
  const Vector3 basePos{position.x, cfg::PLAYER_BASE_Y, position.z};
  const Vector3 topPos{position.x, cfg::PLAYER_BASE_Y + height, position.z};
  DrawCylinderEx(basePos, topPos, radius, radius, cfg::PLAYER_CYLINDER_SIDES,
                 color);
  DrawCylinderWiresEx(basePos, topPos, radius, radius,
                      cfg::PLAYER_CYLINDER_SIDES,
                      ColorBrightness(color, -cfg::PLAYER_WIRE_DARKNESS));
}

float ejectPulseScale(float progress) {
  return 1.0F + ((cfg::PLAYER_EJECT_SCALE_PEAK - 1.0F) *
                 std::sin(progress * std::numbers::pi_v<float>));
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

void PlayerRenderer::loadAssets() {}

void PlayerRenderer::draw(WorldState& world) {
  const auto now = std::chrono::steady_clock::now();

  for (const auto& player : world.players) {
    if (!player.alive) {
      continue;
    }
    const Vector3 position{tileToWorld(player.x), 0.0F, tileToWorld(player.y)};
    const Color color = teamColor(world, player.teamName);

    const auto ejection = std::ranges::find_if(
        world.playerEjections, [&](const PlayerEjection& candidate) {
          return candidate.playerId == player.id;
        });
    if (ejection != world.playerEjections.end()) {
      const float elapsed =
          std::chrono::duration<float>(now - ejection->startTime).count();
      const float progress =
          std::clamp(elapsed / cfg::PLAYER_EJECT_FLASH_DURATION, 0.0F, 1.0F);
      const Color flashColor =
          ColorLerp(cfg::PLAYER_EJECT_FLASH_COLOR, color, progress);
      drawPlayerBody(position, flashColor, ejectPulseScale(progress));
    } else {
      drawPlayerBody(position, color);
    }
    drawOrientationArrow(position, player.orientation);
  }

  std::erase_if(world.playerEjections, [&now](const PlayerEjection& ejection) {
    return std::chrono::duration<float>(now - ejection.startTime).count() >=
           cfg::PLAYER_EJECT_FLASH_DURATION;
  });
}

void PlayerRenderer::drawOverlay(const WorldState& world,
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
             cfg::PLAYER_LABEL_COLOR);
  }
}

void PlayerRenderer::unloadAssets() {}

}  // namespace zappy::gui
