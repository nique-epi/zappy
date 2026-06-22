/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RenderUtils
*/

#include "Render/RenderUtils.hpp"
#include "Render/TileGridConfig.hpp"
#include "Render/WindowConfig.hpp"

namespace zappy::gui {

namespace cfg = config;

float tileToWorld(int gridCoord) {
  return (static_cast<float>(gridCoord) + cfg::TILE_CENTER_OFFSET) *
         cfg::TILE_SIZE;
}

}  // namespace zappy::gui
