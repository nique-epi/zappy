/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** StatsConfig — layout and color tokens for the statistics panel and icon
* button
*/

#pragma once

#include <raylib.h>

namespace zappy::gui::config {

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

constexpr float STATS_BTN_SIZE = 44.0F;
constexpr float STATS_BTN_MARGIN = 12.0F;
constexpr float STATS_BTN_ROUNDNESS = 0.25F;
constexpr float STATS_BTN_BORDER_WIDTH = 2.5F;
constexpr int STATS_BTN_SEGMENTS = 10;
constexpr float STATS_ICON_HOVER_SCALE = 1.20F;
constexpr float STATS_ICON_HELD_SCALE = 0.93F;

constexpr Color STATS_BTN_COLOR = {245, 232, 210, 255};
constexpr Color STATS_BTN_HOVER = {230, 215, 188, 255};
constexpr Color STATS_BTN_HELD = {210, 195, 165, 255};
constexpr Color STATS_BTN_ACTIVE = {200, 175, 130, 255};
constexpr Color STATS_BTN_BORDER = {101, 67, 33, 255};

constexpr int STATS_TOOLTIP_FONT_SIZE = 14;
constexpr float STATS_TOOLTIP_GAP = 8.0F;
constexpr Color STATS_TOOLTIP_COLOR = {255, 255, 255, 255};

constexpr Color STATS_ICON_BAR1 = {190, 75, 55, 240};
constexpr Color STATS_ICON_BAR2 = {215, 140, 35, 240};
constexpr Color STATS_ICON_BAR3 = {175, 195, 45, 240};
constexpr Color STATS_ICON_BAR4 = {55, 160, 75, 240};

constexpr float STATS_PANEL_WIDTH = 360.0F;
constexpr float STATS_PANEL_MARGIN = 12.0F;
constexpr float STATS_PANEL_Y_START = 124.0F;
constexpr float STATS_PANEL_PADDING = 10.0F;
constexpr float STATS_PANEL_CONTENT_W =
    STATS_PANEL_WIDTH - 2.0F * STATS_PANEL_PADDING;
constexpr float STATS_GUI_HEADER_H = 22.0F;

constexpr float STATS_SECTION_H = 22.0F;
constexpr float STATS_SECTION_GAP = 6.0F;
constexpr int STATS_SECTION_FONT = 13;

constexpr float STATS_ROW_H = 18.0F;
constexpr float STATS_TEAM_GAP = 8.0F;
constexpr int STATS_ROW_FONT = 13;

constexpr float STATS_SWATCH_SIZE = 8.0F;
constexpr float STATS_SWATCH_TOP = 5.0F;
constexpr float STATS_SWATCH_GAP = 6.0F;
constexpr float STATS_SWATCH_ROUNDNESS = 0.4F;
constexpr int STATS_SWATCH_SEGMENTS = 4;

constexpr float STATS_CHART_H = 36.0F;
constexpr float STATS_CHART_LABEL_GAP = 2.0F;
constexpr float STATS_CHART_LABELS_H = 12.0F;
constexpr int STATS_MAX_LEVEL = 8;
constexpr float STATS_BAR_GAP = 3.0F;
constexpr int STATS_CHART_LABEL_FONT = 10;

constexpr Color STATS_CHART_EMPTY = {75, 78, 100, 255};
constexpr Color STATS_CHART_LABEL_CLR = {130, 135, 155, 220};

constexpr float STATS_RES_SWATCH_SIZE = 8.0F;
constexpr float STATS_RES_SWATCH_GAP = 5.0F;

constexpr Color STATS_TEXT_SECTION = {120, 120, 120, 255};
constexpr Color STATS_TEXT_LABEL = {65, 65, 65, 255};
constexpr Color STATS_TEXT_VALUE = {25, 25, 25, 255};

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

}  // namespace zappy::gui::config
