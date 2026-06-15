/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Victory — detect a team that has met the end-of-game elevation condition
*/

#pragma once

#include <cstddef>
#include <optional>
#include <string>

namespace zappy::world {

class PlayerRegistry;

/// Drones of one team that must reach the maximum level for that team to win.
constexpr std::size_t dronesRequiredToWin = 6;

/**
 * @brief Team that has met the victory condition, if any.
 *
 * A team wins as soon as at least @ref dronesRequiredToWin of its drones have
 * reached @ref maxPlayerLevel. Meant to be evaluated after each elevation,
 * since only an elevation can raise drones to the winning level.
 *
 * @param[in] players Registry scanned for maxed-out drones, grouped by team.
 * @returns The winning team's name, or std::nullopt when no team qualifies.
 */
[[nodiscard]] std::optional<std::string> findWinningTeam(
    const PlayerRegistry& players);

}  // namespace zappy::world
