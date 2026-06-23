/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** InfoPanel — hovered-tile resource panel driven by mouse raycasting
*/

#include "Render/Panel/InfoPanel.hpp"
#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <format>
#include <string>
#include "Render/Panel/PanelChrome.hpp"
#include "Render/Panel/PanelConfig.hpp"
#include "Render/Panel/ResourceList.hpp"
#include "Render/RenderUtils.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr float planeEpsilon = 1e-6F;

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
  const int tileX = worldToTile(worldX);
  const int tileY = worldToTile(worldZ);
  if (tileY < 0 || tileY >= static_cast<int>(world.tiles.size())) {
    return {.valid = false, .x = 0, .y = 0};
  }
  if (tileX < 0 || tileX >= static_cast<int>(world.tiles[tileY].size())) {
    return {.valid = false, .x = 0, .y = 0};
  }
  return {.valid = true, .x = tileX, .y = tileY};
}

float panelHeight() {
  return cfg::PANEL_HEADER_HEIGHT +
         (static_cast<float>(RESOURCE_COUNT) * cfg::PANEL_ROW_HEIGHT) +
         cfg::INFO_PANEL_BOTTOM_PADDING;
}

Vector2 panelOrigin(Vector2 mouse, float height) {
  const auto screenWidth = static_cast<float>(GetScreenWidth());
  const auto screenHeight = static_cast<float>(GetScreenHeight());
  float originX = mouse.x + cfg::INFO_PANEL_MOUSE_OFFSET;
  float originY = mouse.y + cfg::INFO_PANEL_MOUSE_OFFSET;
  if (originX + cfg::INFO_PANEL_WIDTH > screenWidth) {
    originX = mouse.x - cfg::INFO_PANEL_WIDTH - cfg::INFO_PANEL_MOUSE_OFFSET;
  }
  if (originY + height > screenHeight) {
    originY = screenHeight - height;
  }
  originX =
      std::max(0.0F, std::min(originX, screenWidth - cfg::INFO_PANEL_WIDTH));
  originY = std::max(0.0F, std::min(originY, screenHeight - height));
  return Vector2{originX, originY};
}

void drawHeader(const Rectangle& panel, const Tile& tile) {
  const std::string title = std::format("Tile ({}, {})", tile.x, tile.y);
  DrawText(title.c_str(), static_cast<int>(panel.x + cfg::PANEL_PADDING),
           static_cast<int>(panel.y + cfg::PANEL_PADDING),
           cfg::PANEL_TITLE_FONT_SIZE, cfg::PANEL_TITLE_COLOR);
  const Rectangle accent{
      panel.x + cfg::PANEL_PADDING, panel.y + cfg::PANEL_ACCENT_TOP,
      cfg::INFO_PANEL_CONTENT_WIDTH, cfg::PANEL_ACCENT_THICKNESS};
  DrawRectangleRec(accent, cfg::PANEL_ACCENT_COLOR);
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

  drawPanelBackground(panel);
  drawHeader(panel, tile);
  drawResourceList(
      Vector2{panel.x + cfg::PANEL_PADDING, panel.y + cfg::PANEL_HEADER_HEIGHT},
      cfg::INFO_PANEL_CONTENT_WIDTH, tile.resources);
}

}  // namespace zappy::gui
