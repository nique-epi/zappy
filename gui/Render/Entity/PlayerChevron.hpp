/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerChevron — marker drawn above the hovered and selected players
*/

#pragma once

#include <raylib.h>
#include <optional>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Draws a downward chevron above the hovered and/or selected player.
 *
 * The selected player gets the selection color, the hovered one the hover
 * color; if the same player is both, the selection color wins. Drawn in screen
 * space (always camera-facing) above the player's head.
 */
class PlayerChevron {
 public:
  static void draw(const WorldState& world, const Camera3D& camera,
                   std::optional<int> hoveredId, std::optional<int> selectedId);
};

}  // namespace zappy::gui
