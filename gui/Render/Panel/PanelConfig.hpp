/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PanelConfig — layout and color tokens shared by the GUI panels
*/

#pragma once

#include <raylib.h>
#include "Theme/Theme.hpp"

namespace zappy::gui::config {

constexpr float PANEL_ROUNDNESS = 0.08F;
constexpr int PANEL_SEGMENTS = 10;
constexpr float PANEL_BORDER_THICKNESS = 1.0F;
constexpr float PANEL_SHADOW_OFFSET = 6.0F;
constexpr float PANEL_PADDING = 12.0F;
constexpr float PANEL_HEADER_HEIGHT = 38.0F;
constexpr float PANEL_ACCENT_TOP = 32.0F;
constexpr float PANEL_ACCENT_THICKNESS = 2.0F;
constexpr float PANEL_ROW_HEIGHT = 24.0F;
constexpr float PANEL_ROW_TEXT_TOP = 4.0F;
constexpr int PANEL_TITLE_FONT_SIZE = 18;
constexpr int PANEL_ROW_FONT_SIZE = 16;
constexpr float PANEL_DIM_ALPHA = 0.28F;

constexpr float PANEL_SWATCH_SIZE = 12.0F;
constexpr float PANEL_SWATCH_TOP = 6.0F;
constexpr float PANEL_SWATCH_GAP = 10.0F;
constexpr float PANEL_SWATCH_ROUNDNESS = 0.35F;
constexpr int PANEL_SWATCH_SEGMENTS = 4;

constexpr unsigned char PANEL_BG_ALPHA = 238;
constexpr Color PANEL_BG_COLOR = {theme::parchment.r, theme::parchment.g,
                                  theme::parchment.b, PANEL_BG_ALPHA};
constexpr Color PANEL_BORDER_COLOR = theme::inkBrown;
constexpr Color PANEL_SHADOW_COLOR = {40, 26, 12, 70};
constexpr Color PANEL_TITLE_COLOR = theme::inkBrownDark;
constexpr Color PANEL_LABEL_COLOR = {120, 92, 58, 255};
constexpr Color PANEL_VALUE_COLOR = theme::inkBrownDark;
constexpr Color PANEL_DIM_COLOR = {170, 150, 120, 255};
constexpr Color PANEL_ACCENT_COLOR = theme::inkBrown;
constexpr Color PANEL_SWATCH_BORDER_COLOR = {80, 50, 25, 160};

constexpr Color RESOURCE_FOOD_COLOR = {126, 199, 92, 255};
constexpr Color RESOURCE_LINEMATE_COLOR = {214, 219, 228, 255};
constexpr Color RESOURCE_DERAUMERE_COLOR = {96, 150, 235, 255};
constexpr Color RESOURCE_SIBUR_COLOR = {64, 196, 196, 255};
constexpr Color RESOURCE_MENDIANE_COLOR = {233, 130, 196, 255};
constexpr Color RESOURCE_PHIRAS_COLOR = {235, 175, 76, 255};
constexpr Color RESOURCE_THYSTAME_COLOR = {176, 118, 235, 255};

constexpr float INFO_PANEL_WIDTH = 220.0F;
constexpr float INFO_PANEL_MOUSE_OFFSET = 18.0F;
constexpr float INFO_PANEL_BOTTOM_PADDING = 10.0F;
constexpr float INFO_PANEL_CONTENT_WIDTH =
    INFO_PANEL_WIDTH - PANEL_PADDING - PANEL_PADDING;

constexpr float PLAYER_PANEL_X = 20.0F;
constexpr float PLAYER_PANEL_Y = 108.0F;
constexpr float PLAYER_PANEL_WIDTH = 250.0F;
constexpr float PLAYER_PANEL_BOTTOM_PADDING = 10.0F;
constexpr float PLAYER_PANEL_SECTION_GAP = 8.0F;
constexpr float PLAYER_PANEL_INFO_ROWS = 3.0F;
constexpr float PLAYER_PANEL_CONTENT_WIDTH =
    PLAYER_PANEL_WIDTH - PANEL_PADDING - PANEL_PADDING;
constexpr float PLAYER_PANEL_CLOSE_SIZE = 16.0F;
constexpr float PLAYER_PANEL_CLOSE_TOP = 11.0F;
constexpr float PLAYER_PANEL_CLOSE_THICKNESS = 2.0F;
constexpr Color PLAYER_PANEL_CLOSE_COLOR = {214, 90, 90, 255};
constexpr Color PLAYER_PANEL_CLOSE_HOVER_COLOR = {236, 120, 120, 255};

constexpr float TEAM_PANEL_WIDTH = 240.0F;
constexpr float TEAM_PANEL_MARGIN = 12.0F;
constexpr float TEAM_PANEL_TOP = 128.0F;
constexpr float TEAM_PANEL_BOTTOM_PADDING = 12.0F;
constexpr float TEAM_PANEL_BLOCK_GAP = 12.0F;
constexpr float TEAM_PANEL_NAME_ROW_HEIGHT = 22.0F;
constexpr float TEAM_PANEL_NAME_ALIVE_GAP = 8.0F;
constexpr float TEAM_PANEL_BAR_ROW_HEIGHT = 20.0F;
constexpr float TEAM_PANEL_BAR_HEIGHT = 12.0F;
constexpr float TEAM_PANEL_BAR_LABEL_WIDTH = 72.0F;
constexpr float TEAM_PANEL_BAR_ROUNDNESS = 0.6F;
constexpr int TEAM_PANEL_BAR_SEGMENTS = 6;
constexpr float TEAM_PANEL_VICTORY_BORDER_THICKNESS = 2.0F;
constexpr float TEAM_PANEL_VICTORY_INSET = 4.0F;
constexpr int TEAM_PANEL_BADGE_FONT_SIZE = 13;
constexpr float TEAM_PANEL_CONTENT_WIDTH =
    TEAM_PANEL_WIDTH - PANEL_PADDING - PANEL_PADDING;

constexpr Color TEAM_PANEL_BAR_TRACK_COLOR = {214, 199, 170, 255};
constexpr Color TEAM_PANEL_BAR_FILL_COLOR = {120, 156, 74, 255};
constexpr Color TEAM_PANEL_VICTORY_COLOR = {201, 153, 56, 255};

}  // namespace zappy::gui::config
