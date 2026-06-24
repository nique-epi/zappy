/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceList — shared renderer for the seven-resource list used by panels
*/

#include "Render/Panel/ResourceList.hpp"
#include <raylib.h>
#include <array>
#include <cstddef>
#include <string>
#include "Render/Panel/PanelConfig.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr std::array<const char*, RESOURCE_COUNT> RESOURCE_NAMES = {
    "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"};

constexpr std::array<Color, RESOURCE_COUNT> RESOURCE_COLORS = {
    {cfg::RESOURCE_FOOD_COLOR, cfg::RESOURCE_LINEMATE_COLOR,
     cfg::RESOURCE_DERAUMERE_COLOR, cfg::RESOURCE_SIBUR_COLOR,
     cfg::RESOURCE_MENDIANE_COLOR, cfg::RESOURCE_PHIRAS_COLOR,
     cfg::RESOURCE_THYSTAME_COLOR}};

void drawSwatch(float originX, float rowY, std::size_t index, bool empty) {
  const Rectangle swatch{originX, rowY + cfg::PANEL_SWATCH_TOP,
                         cfg::PANEL_SWATCH_SIZE, cfg::PANEL_SWATCH_SIZE};
  const Color color = empty ? Fade(RESOURCE_COLORS[index], cfg::PANEL_DIM_ALPHA)
                            : RESOURCE_COLORS[index];
  DrawRectangleRounded(swatch, cfg::PANEL_SWATCH_ROUNDNESS,
                       cfg::PANEL_SWATCH_SEGMENTS, color);
  DrawRectangleRoundedLinesEx(
      swatch, cfg::PANEL_SWATCH_ROUNDNESS, cfg::PANEL_SWATCH_SEGMENTS,
      cfg::PANEL_BORDER_THICKNESS, cfg::PANEL_SWATCH_BORDER_COLOR);
}

}  // namespace

void drawResourceList(Vector2 origin, float contentWidth,
                      const std::array<int, RESOURCE_COUNT>& quantities) {
  for (std::size_t i = 0; i < RESOURCE_COUNT; ++i) {
    const float rowY =
        origin.y + (static_cast<float>(i) * cfg::PANEL_ROW_HEIGHT);
    const bool empty = quantities[i] == 0;
    const int textY = static_cast<int>(rowY + cfg::PANEL_ROW_TEXT_TOP);

    drawSwatch(origin.x, rowY, i, empty);

    const Color labelColor =
        empty ? cfg::PANEL_DIM_COLOR : cfg::PANEL_LABEL_COLOR;
    const float nameX =
        origin.x + cfg::PANEL_SWATCH_SIZE + cfg::PANEL_SWATCH_GAP;
    DrawText(RESOURCE_NAMES[i], static_cast<int>(nameX), textY,
             cfg::PANEL_ROW_FONT_SIZE, labelColor);

    const Color valueColor =
        empty ? cfg::PANEL_DIM_COLOR : cfg::PANEL_VALUE_COLOR;
    const std::string value = std::to_string(quantities[i]);
    const int valueWidth = MeasureText(value.c_str(), cfg::PANEL_ROW_FONT_SIZE);
    const float valueX =
        origin.x + contentWidth - static_cast<float>(valueWidth);
    DrawText(value.c_str(), static_cast<int>(valueX), textY,
             cfg::PANEL_ROW_FONT_SIZE, valueColor);
  }
}

}  // namespace zappy::gui
