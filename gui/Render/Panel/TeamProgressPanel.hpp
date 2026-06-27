/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TeamProgressPanel — permanent scoreboard of each team's victory progress
*/

#pragma once

namespace zappy::gui {

struct WorldState;

/**
 * @brief Always-visible scoreboard ranking every known team's race to win.
 *
 * Lists each team received through @c tna in a fixed top-right panel. For every
 * team it shows a color swatch, the team name, its living-player count, and how
 * many of those players have reached @ref MAX_PLAYER_LEVEL, with a progress bar
 * toward @ref PLAYERS_REQUIRED_FOR_VICTORY. Teams that meet the victory
 * condition (or that the server declared winners through @c seg) are
 * highlighted. Stateless and redrawn every frame, so it tracks @c pnw, @c pdi
 * and @c plv live.
 */
class TeamProgressPanel {
 public:
  /**
   * @brief Draw the scoreboard for the current world state.
   *
   * @param[in] world Live world state queried for teams and players.
   */
  static void draw(const WorldState& world);
};

}  // namespace zappy::gui
