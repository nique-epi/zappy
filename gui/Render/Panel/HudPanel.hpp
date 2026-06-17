/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** HudPanel — always-on overlay with the global session / world summary
*/

#pragma once

namespace zappy::gui {

struct GuiConfig;
struct WorldState;

/**
 * @brief Always-visible heads-up display drawn on top of the 3D view.
 *
 * Renders the window title and a one-line summary (server endpoint, map
 * dimensions, player and team counts) in the top-left corner. Stateless: it
 * reads everything from its arguments and is redrawn every frame.
 */
class HudPanel {
 public:
  /**
   * @brief Draw the HUD overlay.
   *
   * @param[in] config Parsed CLI configuration (server endpoint).
   * @param[in] world  Live world state (map size, players, teams).
   */
  static void draw(const GuiConfig& config, const WorldState& world);
};

}  // namespace zappy::gui
