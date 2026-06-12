/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Broadcast — tile a drone hears an anonymous sound coming from
*/

#pragma once

#include "App/World/Player/Direction.hpp"

namespace zappy::world {

/**
 * @brief Tile number K a receiver hears an anonymous broadcast coming from.
 *
 * K numbers the eight tiles around the receiver: 1 is the tile in front of it,
 * then 2..8 wind counter-clockwise (front-left, left, back-left, back,
 * back-right, right, front-right). K is 0 when the emitter stands on the
 * receiver's own tile. The emitter is reached along the shortest toroidal path,
 * so the world's wrap-around edges are taken into account.
 *
 * @param[in] emitterX       Column of the broadcasting drone.
 * @param[in] emitterY       Row of the broadcasting drone.
 * @param[in] receiverX      Column of the listening drone.
 * @param[in] receiverY      Row of the listening drone.
 * @param[in] receiverFacing Orientation of the listening drone.
 * @param[in] width          World width, used to wrap the column distance.
 * @param[in] height         World height, used to wrap the row distance.
 * @returns The K code in [0, 8] from the receiver's point of view.
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
[[nodiscard]] int broadcastSoundDirection(int emitterX, int emitterY,
                                          int receiverX, int receiverY,
                                          Direction receiverFacing, int width,
                                          int height);

}  // namespace zappy::world
