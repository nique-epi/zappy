/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RenderUtils
*/

#include "Render/RenderUtils.hpp"
#include <cmath>
#include "Render/TileGridConfig.hpp"
#include "Render/WindowConfig.hpp"

namespace zappy::gui {

namespace cfg = config;

float tileToWorld(int gridCoord) {
  return (static_cast<float>(gridCoord) + cfg::TILE_CENTER_OFFSET) *
         cfg::TILE_SIZE;
}

int worldToTile(float worldCoord) {
  return static_cast<int>(std::floor(worldCoord / cfg::TILE_SIZE));
}

}  // namespace zappy::gui
