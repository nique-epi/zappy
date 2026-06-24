/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerPicker — finds the on-screen player closest to the mouse cursor
*/

#include "Render/Entity/PlayerPicker.hpp"
#include <raylib.h>
#include <optional>
#include "Render/Entity/PlayerRendererConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

bool inFrontOfCamera(const Camera3D& camera, Vector3 point) {
  const float forwardX = camera.target.x - camera.position.x;
  const float forwardY = camera.target.y - camera.position.y;
  const float forwardZ = camera.target.z - camera.position.z;
  const float toPointX = point.x - camera.position.x;
  const float toPointY = point.y - camera.position.y;
  const float toPointZ = point.z - camera.position.z;
  const float dot =
      (forwardX * toPointX) + (forwardY * toPointY) + (forwardZ * toPointZ);
  return dot > 0.0F;
}

}  // namespace

std::optional<int> PlayerPicker::nearest(const WorldState& world,
                                         const Camera3D& camera) {
  const Vector2 mouse = GetMousePosition();
  std::optional<int> best;
  float bestDistance = cfg::PLAYER_PICK_RADIUS * cfg::PLAYER_PICK_RADIUS;

  for (const auto& player : world.players) {
    if (!player.alive) {
      continue;
    }
    const Vector3 position{tileToWorld(player.x), cfg::PLAYER_CENTER_Y,
                           tileToWorld(player.y)};
    if (!inFrontOfCamera(camera, position)) {
      continue;
    }
    const Vector2 screen = GetWorldToScreen(position, camera);
    const float deltaX = screen.x - mouse.x;
    const float deltaY = screen.y - mouse.y;
    const float distance = (deltaX * deltaX) + (deltaY * deltaY);
    if (distance < bestDistance) {
      bestDistance = distance;
      best = player.id;
    }
  }
  return best;
}

}  // namespace zappy::gui
