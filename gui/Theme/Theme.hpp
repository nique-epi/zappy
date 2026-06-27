/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Theme — canonical parchment/ink color palette shared by menu and panels
*/

#pragma once

#include <raylib.h>

namespace zappy::gui::theme {

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
constexpr Color parchment = {245, 232, 210, 255};
constexpr Color parchmentLight = {255, 248, 235, 255};
constexpr Color parchmentHover = {230, 215, 188, 255};
constexpr Color parchmentPressed = {210, 195, 165, 255};
constexpr Color inkBrown = {101, 67, 33, 255};
constexpr Color inkBrownDark = {80, 50, 25, 255};
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

}  // namespace zappy::gui::theme
