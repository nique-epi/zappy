/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerPanel — fixed detail panel for the selected player
*/

#pragma once

#include <raylib.h>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Fixed-position detail panel for the currently selected player.
 *
 * Shows the player's id, team, level, orientation and full inventory, read
 * live from @ref WorldState each frame. Drawn only while a player is selected.
 */
class PlayerPanel {
 public:
  /**
   * @brief Draw the panel for @p playerId.
   *
   * @returns true if the close button was clicked this frame.
   */
  static bool draw(const WorldState& world, int playerId);

  /**
   * @brief Whether @p point falls inside the (fixed) panel bounds.
   */
  static bool contains(Vector2 point);
};

}  // namespace zappy::gui
