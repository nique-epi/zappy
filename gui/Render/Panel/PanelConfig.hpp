/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PanelConfig — layout constants shared by the Raygui panels
*/

#pragma once

namespace zappy::gui::config {

constexpr float INFO_PANEL_WIDTH = 200.0F;
constexpr float INFO_PANEL_PADDING = 8.0F;
constexpr float INFO_PANEL_HEADER_HEIGHT = 28.0F;
constexpr float INFO_PANEL_ROW_HEIGHT = 22.0F;
constexpr float INFO_PANEL_MOUSE_OFFSET = 16.0F;
constexpr float INFO_PANEL_CONTENT_WIDTH =
    INFO_PANEL_WIDTH - INFO_PANEL_PADDING - INFO_PANEL_PADDING;

}  // namespace zappy::gui::config
