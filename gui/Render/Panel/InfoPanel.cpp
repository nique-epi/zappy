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
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr float planeEpsilon = 1e-6F;

constexpr std::array<const char*, RESOURCE_COUNT> RESOURCE_NAMES = {
    "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"};

constexpr std::array<Color, RESOURCE_COUNT> RESOURCE_COLORS = {
    {cfg::RESOURCE_FOOD_COLOR, cfg::RESOURCE_LINEMATE_COLOR,
     cfg::RESOURCE_DERAUMERE_COLOR, cfg::RESOURCE_SIBUR_COLOR,
     cfg::RESOURCE_MENDIANE_COLOR, cfg::RESOURCE_PHIRAS_COLOR,
     cfg::RESOURCE_THYSTAME_COLOR}};

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
         cfg::INFO_PANEL_BOTTOM_PADDING;
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

void drawBackground(const Rectangle& panel) {
  const Rectangle shadow{panel.x + cfg::INFO_PANEL_SHADOW_OFFSET,
                         panel.y + cfg::INFO_PANEL_SHADOW_OFFSET, panel.width,
                         panel.height};
  DrawRectangleRounded(shadow, cfg::INFO_PANEL_ROUNDNESS,
                       cfg::INFO_PANEL_SEGMENTS, cfg::INFO_PANEL_SHADOW_COLOR);
  DrawRectangleRounded(panel, cfg::INFO_PANEL_ROUNDNESS,
                       cfg::INFO_PANEL_SEGMENTS, cfg::INFO_PANEL_BG_COLOR);
  DrawRectangleRoundedLinesEx(
      panel, cfg::INFO_PANEL_ROUNDNESS, cfg::INFO_PANEL_SEGMENTS,
      cfg::INFO_PANEL_BORDER_THICKNESS, cfg::INFO_PANEL_BORDER_COLOR);
}

void drawHeader(const Rectangle& panel, const Tile& tile) {
  const std::string title = std::format("Tile ({}, {})", tile.x, tile.y);
  DrawText(title.c_str(), static_cast<int>(panel.x + cfg::INFO_PANEL_PADDING),
           static_cast<int>(panel.y + cfg::INFO_PANEL_PADDING),
           cfg::INFO_PANEL_TITLE_FONT_SIZE, cfg::INFO_PANEL_TITLE_COLOR);
  const Rectangle accent{
      panel.x + cfg::INFO_PANEL_PADDING, panel.y + cfg::INFO_PANEL_ACCENT_TOP,
      cfg::INFO_PANEL_CONTENT_WIDTH, cfg::INFO_PANEL_ACCENT_THICKNESS};
  DrawRectangleRec(accent, cfg::INFO_PANEL_ACCENT_COLOR);
}

void drawResourceRow(const Rectangle& panel, std::size_t index, int quantity) {
  const float rowY = panel.y + cfg::INFO_PANEL_HEADER_HEIGHT +
                     (static_cast<float>(index) * cfg::INFO_PANEL_ROW_HEIGHT);
  const bool empty = quantity == 0;

  const Rectangle swatch{
      panel.x + cfg::INFO_PANEL_PADDING, rowY + cfg::INFO_PANEL_SWATCH_TOP,
      cfg::INFO_PANEL_SWATCH_SIZE, cfg::INFO_PANEL_SWATCH_SIZE};
  const Color swatchColor =
      empty ? Fade(RESOURCE_COLORS[index], cfg::INFO_PANEL_DIM_ALPHA)
            : RESOURCE_COLORS[index];
  DrawRectangleRounded(swatch, cfg::INFO_PANEL_SWATCH_ROUNDNESS,
                       cfg::INFO_PANEL_SWATCH_SEGMENTS, swatchColor);
  DrawRectangleRoundedLinesEx(
      swatch, cfg::INFO_PANEL_SWATCH_ROUNDNESS, cfg::INFO_PANEL_SWATCH_SEGMENTS,
      cfg::INFO_PANEL_BORDER_THICKNESS, cfg::INFO_PANEL_SWATCH_BORDER_COLOR);

  const Color labelColor =
      empty ? cfg::INFO_PANEL_DIM_COLOR : cfg::INFO_PANEL_LABEL_COLOR;
  const float nameX = panel.x + cfg::INFO_PANEL_PADDING +
                      cfg::INFO_PANEL_SWATCH_SIZE + cfg::INFO_PANEL_SWATCH_GAP;
  DrawText(RESOURCE_NAMES[index], static_cast<int>(nameX),
           static_cast<int>(rowY + cfg::INFO_PANEL_ROW_TEXT_TOP),
           cfg::INFO_PANEL_ROW_FONT_SIZE, labelColor);

  const Color valueColor =
      empty ? cfg::INFO_PANEL_DIM_COLOR : cfg::INFO_PANEL_VALUE_COLOR;
  const std::string value = std::to_string(quantity);
  const int valueWidth =
      MeasureText(value.c_str(), cfg::INFO_PANEL_ROW_FONT_SIZE);
  const float valueX = panel.x + cfg::INFO_PANEL_WIDTH -
                       cfg::INFO_PANEL_PADDING - static_cast<float>(valueWidth);
  DrawText(value.c_str(), static_cast<int>(valueX),
           static_cast<int>(rowY + cfg::INFO_PANEL_ROW_TEXT_TOP),
           cfg::INFO_PANEL_ROW_FONT_SIZE, valueColor);
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

  drawBackground(panel);
  drawHeader(panel, tile);
  for (std::size_t i = 0; i < RESOURCE_COUNT; ++i) {
    drawResourceRow(panel, i, tile.resources[i]);
  }
}

}  // namespace zappy::gui
