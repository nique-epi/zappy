/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TileGridRenderer
*/

#include "Render/TileGridRenderer.hpp"
#include "Render/TileGridConfig.hpp"
#include "Render/WindowConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

Vector3 tileCenter(int gridX, int gridY) {
  return Vector3{
      (static_cast<float>(gridX) + cfg::TILE_CENTER_OFFSET) * cfg::TILE_SIZE,
      -cfg::TILE_HALF_HEIGHT,
      (static_cast<float>(gridY) + cfg::TILE_CENTER_OFFSET) * cfg::TILE_SIZE,
  };
}

void drawOneTile(int gridX, int gridY, Color fillColor) {
  const Vector3 center = tileCenter(gridX, gridY);
  DrawCube(center, cfg::TILE_SIZE, cfg::TILE_HEIGHT, cfg::TILE_SIZE, fillColor);
  DrawCubeWires(center, cfg::TILE_SIZE, cfg::TILE_HEIGHT, cfg::TILE_SIZE,
                cfg::WIRE_COLOR);
}

Color mainColor(int tileX, int tileY) {
  return ((tileX + tileY) % 2 == 0) ? cfg::MAIN_EVEN_COLOR
                                    : cfg::MAIN_ODD_COLOR;
}

void drawToroidalBorder(const WorldState& world) {
  const float totalW = static_cast<float>(world.width) * cfg::TILE_SIZE;
  const float totalD = static_cast<float>(world.height) * cfg::TILE_SIZE;
  const float centerY = -cfg::TILE_HALF_HEIGHT;
  const float halfW = totalW * cfg::TILE_CENTER_OFFSET;
  const float halfD = totalD * cfg::TILE_CENTER_OFFSET;

  DrawCube(Vector3{-cfg::BORDER_HALF_THICKNESS, centerY, halfD},
           cfg::BORDER_THICKNESS, cfg::TILE_HEIGHT, totalD, cfg::BORDER_COLOR);
  DrawCube(Vector3{totalW + cfg::BORDER_HALF_THICKNESS, centerY, halfD},
           cfg::BORDER_THICKNESS, cfg::TILE_HEIGHT, totalD, cfg::BORDER_COLOR);
  DrawCube(Vector3{halfW, centerY, -cfg::BORDER_HALF_THICKNESS}, totalW,
           cfg::TILE_HEIGHT, cfg::BORDER_THICKNESS, cfg::BORDER_COLOR);
  DrawCube(Vector3{halfW, centerY, totalD + cfg::BORDER_HALF_THICKNESS}, totalW,
           cfg::TILE_HEIGHT, cfg::BORDER_THICKNESS, cfg::BORDER_COLOR);
}

}  // namespace

void TileGridRenderer::draw(const WorldState& world) {
  for (int tileY = 0; tileY < world.height; ++tileY) {
    for (int tileX = 0; tileX < world.width; ++tileX) {
      drawOneTile(tileX, tileY, mainColor(tileX, tileY));
    }
  }
  drawToroidalBorder(world);
}

}  // namespace zappy::gui
