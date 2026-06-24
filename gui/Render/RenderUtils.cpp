/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RenderUtils — shared geometry and world-query helpers for the renderers
*/

#include "Render/RenderUtils.hpp"
#include <cmath>
#include <cstddef>
#include <string>
#include "Render/Entity/PlayerRendererConfig.hpp"
#include "Render/TileGridConfig.hpp"
#include "Render/WindowConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

float tileToWorld(int gridCoord) {
  return (static_cast<float>(gridCoord) + cfg::TILE_CENTER_OFFSET) *
         cfg::TILE_SIZE;
}

int worldToTile(float worldCoord) {
  return static_cast<int>(std::floor(worldCoord / cfg::TILE_SIZE));
}

Color teamColor(const WorldState& world, const std::string& teamName) {
  for (std::size_t i = 0; i < world.teams.size(); ++i) {
    if (world.teams[i] == teamName) {
      return cfg::TEAM_COLORS[i % cfg::TEAM_COLORS.size()];
    }
  }
  return cfg::TEAM_COLORS[0];
}

}  // namespace zappy::gui
