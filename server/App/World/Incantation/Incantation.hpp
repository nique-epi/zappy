/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Incantation — resolve an elevation ritual on a tile
*/

#pragma once

#include <vector>

namespace zappy::world {

class Map;
class PlayerRegistry;

/**
 * @brief Outcome of an elevation ritual resolution.
 *
 * On success the ritual's stones have already been consumed from the tile and
 * every participant has been raised one level.
 */
struct IncantationOutcome {
  /// Whether the prerequisites were met and the elevation went through.
  bool succeeded;
  /// Level the participants reached on success; meaningless when @c succeeded
  /// is false.
  int newLevel;
  /// Identifiers of every drone raised by the ritual, initiator included; empty
  /// on failure.
  std::vector<int> participants;
};

/**
 * @brief Resolve the elevation ritual started by drone @p initiatorId.
 *
 * Gathers every drone sharing the initiator's tile and level, then checks the
 * subject's prerequisites for that level (enough same-level players and enough
 * minerals on the tile). On success the required minerals are removed from the
 * tile and every gathered drone is raised one level. On failure nothing is
 * mutated.
 *
 * @param[in]     initiatorId Drone that issued the Incantation.
 * @param[in,out] players     Registry the drones are looked up and raised in.
 * @param[in,out] map         World whose tile is inspected and drained.
 * @returns Whether the ritual succeeded, the reached level, and the raised
 *          drones.
 */
[[nodiscard]] IncantationOutcome getIncantationOutcome(int initiatorId,
                                                       PlayerRegistry& players,
                                                       Map& map);

}  // namespace zappy::world
