/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Ejection — push every other drone off a tile and wipe the eggs on it
*/

#pragma once

#include <cstddef>
#include <vector>

namespace zappy::world {

class Map;
class PlayerRegistry;
class TeamRegistry;

/**
 * @brief A drone that was pushed by an ejection, with the code it must hear.
 */
struct EjectedDrone {
  int playerId;
  int code;
};

/**
 * @brief Outcome of an ejection: who was pushed and which eggs died.
 */
struct EjectOutcome {
  std::vector<EjectedDrone> ejected;
  std::vector<int> destroyedEggs;
};

/**
 * @brief Eject every drone other than @p ejectorId from the ejector's tile.
 *
 * Each pushed drone is moved one tile along the ejector's facing direction
 * (coordinates wrap on the toroidal @p map) and recorded with its `eject: K`
 * code. Every egg sitting on the ejector's tile is destroyed: removed from its
 * owning team via @p teams and from the tile. The ejector itself does not move.
 *
 * @param[in]     ejectorId Identifier of the drone issuing the eject.
 * @param[in,out] players   Registry the drones are looked up and moved in.
 * @param[in,out] map       World whose tiles are updated.
 * @param[in,out] teams     Registry the destroyed eggs are removed from.
 * @returns The pushed drones (empty when the ejector is gone or alone) and the
 *          identifiers of the destroyed eggs.
 */
[[nodiscard]] EjectOutcome ejectFromTile(int ejectorId, PlayerRegistry& players,
                                         Map& map, TeamRegistry& teams);

}  // namespace zappy::world
