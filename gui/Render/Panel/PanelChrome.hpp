/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PanelChrome — shared rounded background / shadow / border for GUI panels
*/

#pragma once

#include <raylib.h>

namespace zappy::gui {

/**
 * @brief Draw the shared panel chrome: drop shadow, rounded fill, and border.
 *
 * @param[in] panel Panel bounds in screen space.
 */
void drawPanelBackground(const Rectangle& panel);

}  // namespace zappy::gui
