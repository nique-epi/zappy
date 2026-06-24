/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerChevron — marker drawn above the hovered and selected players
*/

#include "Render/Entity/PlayerChevron.hpp"
#include <raylib.h>
#include <optional>
#include "Render/Entity/PlayerRendererConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "World/WorldQuery.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

void drawChevronAbove(const WorldState& world, const Camera3D& camera,
                      int playerId, Color color) {
  const Player* player = findPlayerById(world, playerId);
  if (player == nullptr || !player->alive) {
    return;
  }
  const Vector3 head{tileToWorld(player->x), cfg::PLAYER_CHEVRON_Y,
                     tileToWorld(player->y)};
  const Vector2 tip = GetWorldToScreen(head, camera);
  const Vector2 left{tip.x - cfg::PLAYER_CHEVRON_HALF_WIDTH,
                     tip.y - cfg::PLAYER_CHEVRON_HEIGHT};
  const Vector2 right{tip.x + cfg::PLAYER_CHEVRON_HALF_WIDTH,
                      tip.y - cfg::PLAYER_CHEVRON_HEIGHT};
  DrawLineEx(left, tip, cfg::PLAYER_CHEVRON_THICKNESS, color);
  DrawLineEx(tip, right, cfg::PLAYER_CHEVRON_THICKNESS, color);
}

}  // namespace

void PlayerChevron::draw(const WorldState& world, const Camera3D& camera,
                         std::optional<int> hoveredId,
                         std::optional<int> selectedId) {
  if (selectedId.has_value()) {
    drawChevronAbove(world, camera, *selectedId,
                     cfg::PLAYER_CHEVRON_SELECTED_COLOR);
  }
  if (hoveredId.has_value() && hoveredId != selectedId) {
    drawChevronAbove(world, camera, *hoveredId,
                     cfg::PLAYER_CHEVRON_HOVER_COLOR);
  }
}

}  // namespace zappy::gui
