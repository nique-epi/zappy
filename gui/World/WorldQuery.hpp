/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldQuery — read-only lookups over WorldState (no rendering dependency)
*/

#pragma once

#include <string>
#include <vector>
#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Find the player with @p playerId, or nullptr if absent.
 *
 * Lives outside RenderUtils so consumers that only query the world (e.g.
 * @ref PlayerSelection) stay free of any raylib dependency and unit-testable.
 */
const Player* findPlayerById(const WorldState& world, int playerId);

/**
 * @brief Victory-progress summary for a single team.
 */
struct TeamProgress {
  std::string name;
  int aliveCount;
  int maxLevelCount;
};

/**
 * @brief Summarize each known team's progress toward the elevation goal.
 *
 * Walks @ref WorldState::teams in @c tna arrival order and, for every team,
 * counts its living players and how many of them have reached
 * @ref MAX_PLAYER_LEVEL. Raylib-free so the team panel logic stays
 * unit-testable.
 *
 * @param[in] world Live world state (teams and players).
 * @returns One @ref TeamProgress per known team, in arrival order.
 */
std::vector<TeamProgress> computeTeamProgress(const WorldState& world);

}  // namespace zappy::gui
