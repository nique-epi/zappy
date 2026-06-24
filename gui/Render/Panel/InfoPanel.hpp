/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** InfoPanel — hovered-tile resource panel driven by mouse raycasting
*/

#pragma once

#include <raylib.h>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Floating panel showing the resources of the tile under the mouse.
 *
 * Casts a ray from the mouse position into the 3D scene, intersects it with
 * the tile plane (y = 0, matching @ref TileGridRenderer), and—when a tile is
 * hovered—draws a styled tooltip near the cursor listing the seven resource
 * quantities (food, linemate, deraumere, sibur, mendiane, phiras, thystame),
 * each with a color swatch and the empty ones dimmed.
 *
 * Display-only: it draws nothing when no tile is hovered and never captures
 * input, so camera and player interactions are left untouched. Stateless and
 * redrawn every frame, so resource changes appear in real time.
 */
class InfoPanel {
 public:
  /**
   * @brief Draw the hovered-tile panel, or nothing if no tile is hovered.
   *
   * @param[in] world  Live world state queried for the hovered tile.
   * @param[in] camera Active 3D camera, used to unproject the mouse ray.
   */
  static void draw(const WorldState& world, const Camera3D& camera);
};

}  // namespace zappy::gui
