/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** InfoPanel — hovered-tile resource panel driven by mouse raycasting
*/

#include "Render/Panel/InfoPanel.hpp"
#include <raylib.h>
#include <array>
#include <cmath>
#include <cstddef>
#include <format>
#include <string>
#include "Render/Panel/PanelConfig.hpp"
#include "Render/WindowConfig.hpp"
#include "Render/raygui.h"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr float planeEpsilon = 1e-6F;

constexpr std::array<const char*, RESOURCE_COUNT> RESOURCE_NAMES = {
    "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"};

struct HoveredTile {
  bool valid;
  int x;
  int y;
};

HoveredTile getTile(const WorldState& world, const Camera3D& camera) {
  const Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
  if (std::fabs(ray.direction.y) < planeEpsilon) {
    return {.valid = false, .x = 0, .y = 0};
  }
  const float distance = -ray.position.y / ray.direction.y;
  if (distance < 0.0F) {
    return {.valid = false, .x = 0, .y = 0};
  }
  const float worldX = ray.position.x + (distance * ray.direction.x);
  const float worldZ = ray.position.z + (distance * ray.direction.z);
  const int tileX = static_cast<int>(std::floor(worldX / cfg::TILE_SIZE));
  const int tileY = static_cast<int>(std::floor(worldZ / cfg::TILE_SIZE));
  if (tileY < 0 || tileY >= static_cast<int>(world.tiles.size())) {
    return {.valid = false, .x = 0, .y = 0};
  }
  if (tileX < 0 || tileX >= static_cast<int>(world.tiles[tileY].size())) {
    return {.valid = false, .x = 0, .y = 0};
  }
  return {.valid = true, .x = tileX, .y = tileY};
}

float panelHeight() {
  return cfg::INFO_PANEL_HEADER_HEIGHT +
         (static_cast<float>(RESOURCE_COUNT) * cfg::INFO_PANEL_ROW_HEIGHT) +
         cfg::INFO_PANEL_PADDING;
}

Vector2 panelOrigin(Vector2 mouse, float height) {
  float originX = mouse.x + cfg::INFO_PANEL_MOUSE_OFFSET;
  float originY = mouse.y + cfg::INFO_PANEL_MOUSE_OFFSET;
  if (originX + cfg::INFO_PANEL_WIDTH > static_cast<float>(GetScreenWidth())) {
    originX = mouse.x - cfg::INFO_PANEL_WIDTH - cfg::INFO_PANEL_MOUSE_OFFSET;
  }
  if (originY + height > static_cast<float>(GetScreenHeight())) {
    originY = static_cast<float>(GetScreenHeight()) - height;
  }
  return Vector2{originX, originY};
}

}  // namespace

void InfoPanel::draw(const WorldState& world, const Camera3D& camera) {
  const HoveredTile hovered = getTile(world, camera);
  if (!hovered.valid) {
    return;
  }

  const Tile& tile = world.tiles[hovered.y][hovered.x];
  const float height = panelHeight();
  const Vector2 origin = panelOrigin(GetMousePosition(), height);
  const Rectangle panel{origin.x, origin.y, cfg::INFO_PANEL_WIDTH, height};

  const std::string title = std::format("Tile ({}, {})", tile.x, tile.y);
  GuiPanel(panel, title.c_str());

  for (std::size_t i = 0; i < RESOURCE_COUNT; ++i) {
    const Rectangle row{
        panel.x + cfg::INFO_PANEL_PADDING,
        panel.y + cfg::INFO_PANEL_HEADER_HEIGHT +
            (static_cast<float>(i) * cfg::INFO_PANEL_ROW_HEIGHT),
        cfg::INFO_PANEL_CONTENT_WIDTH, cfg::INFO_PANEL_ROW_HEIGHT};
    const std::string text =
        std::format("{}: {}", RESOURCE_NAMES[i], tile.resources[i]);
    GuiLabel(row, text.c_str());
  }
}

}  // namespace zappy::gui
