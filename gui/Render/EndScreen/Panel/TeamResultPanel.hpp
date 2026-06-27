/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TeamResultPanel — centered team results panel for the end-of-game screen
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <raylib.h>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Draws the centered rounded panel listing all teams.
 *
 * Winner row: green accent bar + "WINNER" badge.
 * Other rows:  grey accent bar + "DEFEATED" badge.
 *
 * @param[in] world Live world state (teams, winnerTeam).
 * @param[in] font  KiwiSoda font pre-loaded by EndScreen.
 */
class TeamResultPanel {
 public:
  static void draw(const WorldState& world, const Font& font);
};

}  // namespace zappy::gui
