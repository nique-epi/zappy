/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Movement — advance a drone across the toroidal world
*/

#pragma once

namespace zappy::world {

class Map;
class Player;

/**
 * @brief Advance @p player one tile along its current orientation.
 *
 * The destination is normalised on the toroidal @p map, so leaving an edge
 * re-enters the opposite one. The tile bookkeeping is kept in sync: the player
 * is removed from its old tile and recorded on the new one.
 *
 * @param[in,out] player Drone to move; its position is updated.
 * @param[in,out] map    World whose tiles are updated.
 */
void moveForward(Player& player, Map& map);

}  // namespace zappy::world
