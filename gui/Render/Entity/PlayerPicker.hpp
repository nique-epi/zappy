/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerPicker — finds the on-screen player closest to the mouse cursor
*/

#pragma once

#include <raylib.h>
#include <optional>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Resolves which player the mouse is pointing at.
 *
 * Projects every alive player to screen space and returns the id of the one
 * closest to the cursor within a pixel radius (or none). O(n) over players —
 * one projection and a squared-distance compare each — which is negligible for
 * Zappy's entity counts, so no spatial index is needed.
 */
class PlayerPicker {
 public:
  /**
   * @brief Id of the alive player nearest the mouse, or none if none is close.
   */
  static std::optional<int> nearest(const WorldState& world,
                                    const Camera3D& camera);
};

}  // namespace zappy::gui
