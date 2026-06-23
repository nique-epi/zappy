/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EggRendererConfig
*/

#pragma once

#include <raylib.h>

namespace zappy::gui::config {

constexpr float EGG_RADIUS = 0.13F;
constexpr float EGG_CENTER_Y = 0.13F;
constexpr int EGG_RINGS = 8;
constexpr int EGG_SLICES = 8;
constexpr Color EGG_FILL_COLOR = {255, 245, 180, 255};
constexpr Color EGG_WIRE_COLOR = {160, 130, 40, 220};

}  // namespace zappy::gui::config
