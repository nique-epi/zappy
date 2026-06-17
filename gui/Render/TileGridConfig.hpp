/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TileGridConfig
*/

#pragma once

#include <raylib.h>

namespace zappy::gui::config {

constexpr float TILE_HEIGHT = 0.2F;
constexpr float TILE_HALF_HEIGHT = TILE_HEIGHT / 2.0F;
constexpr float TILE_CENTER_OFFSET = 0.5F;
constexpr float BORDER_THICKNESS = 0.3F;
constexpr float BORDER_HALF_THICKNESS = BORDER_THICKNESS / 2.0F;

constexpr Color MAIN_EVEN_COLOR = {175, 128, 70, 255};
constexpr Color MAIN_ODD_COLOR = {130, 88, 40, 255};
constexpr Color WIRE_COLOR = {30, 18, 8, 255};
constexpr Color BORDER_COLOR = {220, 120, 60, 255};

}  // namespace zappy::gui::config
