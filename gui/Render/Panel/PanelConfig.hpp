/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PanelConfig — layout and color tokens shared by the Raygui panels
*/

#pragma once

#include <raylib.h>

namespace zappy::gui::config {

constexpr float INFO_PANEL_WIDTH = 220.0F;
constexpr float INFO_PANEL_PADDING = 12.0F;
constexpr float INFO_PANEL_HEADER_HEIGHT = 38.0F;
constexpr float INFO_PANEL_ROW_HEIGHT = 24.0F;
constexpr float INFO_PANEL_BOTTOM_PADDING = 10.0F;
constexpr float INFO_PANEL_MOUSE_OFFSET = 18.0F;
constexpr float INFO_PANEL_CONTENT_WIDTH =
    INFO_PANEL_WIDTH - INFO_PANEL_PADDING - INFO_PANEL_PADDING;

constexpr float INFO_PANEL_ROUNDNESS = 0.08F;
constexpr int INFO_PANEL_SEGMENTS = 10;
constexpr float INFO_PANEL_BORDER_THICKNESS = 1.0F;
constexpr float INFO_PANEL_SHADOW_OFFSET = 6.0F;
constexpr float INFO_PANEL_ACCENT_TOP = 32.0F;
constexpr float INFO_PANEL_ACCENT_THICKNESS = 2.0F;

constexpr float INFO_PANEL_SWATCH_SIZE = 12.0F;
constexpr float INFO_PANEL_SWATCH_TOP = 6.0F;
constexpr float INFO_PANEL_SWATCH_GAP = 10.0F;
constexpr float INFO_PANEL_SWATCH_ROUNDNESS = 0.35F;
constexpr int INFO_PANEL_SWATCH_SEGMENTS = 4;

constexpr float INFO_PANEL_ROW_TEXT_TOP = 4.0F;
constexpr int INFO_PANEL_TITLE_FONT_SIZE = 18;
constexpr int INFO_PANEL_ROW_FONT_SIZE = 16;
constexpr float INFO_PANEL_DIM_ALPHA = 0.28F;

constexpr Color INFO_PANEL_BG_COLOR = {26, 27, 38, 240};
constexpr Color INFO_PANEL_BORDER_COLOR = {60, 63, 84, 255};
constexpr Color INFO_PANEL_SHADOW_COLOR = {0, 0, 0, 90};
constexpr Color INFO_PANEL_TITLE_COLOR = {236, 239, 244, 255};
constexpr Color INFO_PANEL_LABEL_COLOR = {168, 178, 200, 255};
constexpr Color INFO_PANEL_VALUE_COLOR = {236, 239, 244, 255};
constexpr Color INFO_PANEL_DIM_COLOR = {92, 99, 116, 255};
constexpr Color INFO_PANEL_ACCENT_COLOR = {122, 162, 247, 255};
constexpr Color INFO_PANEL_SWATCH_BORDER_COLOR = {12, 12, 18, 140};

constexpr Color RESOURCE_FOOD_COLOR = {126, 199, 92, 255};
constexpr Color RESOURCE_LINEMATE_COLOR = {214, 219, 228, 255};
constexpr Color RESOURCE_DERAUMERE_COLOR = {96, 150, 235, 255};
constexpr Color RESOURCE_SIBUR_COLOR = {64, 196, 196, 255};
constexpr Color RESOURCE_MENDIANE_COLOR = {233, 130, 196, 255};
constexpr Color RESOURCE_PHIRAS_COLOR = {235, 175, 76, 255};
constexpr Color RESOURCE_THYSTAME_COLOR = {176, 118, 235, 255};

}  // namespace zappy::gui::config
