/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Direction — cardinal orientation of a drone on the toroidal world
*/

#pragma once

#include <cstdint>
#include <string_view>

namespace zappy::world {

/**
 * @brief Cardinal orientation of a drone.
 *
 * Encoded in protocol order (1=N, 2=E, 3=S, 4=W) so the value can be written
 * directly to the GUI `ppo` message.
 */
enum class Direction : std::uint8_t {
  North = 1,
  East = 2,
  South = 3,
  West = 4,
};

/**
 * @brief Per-axis step a drone takes when moving one tile along @p direction.
 *
 * North decreases the row, South increases it; East increases the column, West
 * decreases it. Coordinates are meant to be normalised by the toroidal map.
 */
struct DirectionOffset {
  int dx;
  int dy;
};

/**
 * @brief Single-letter mnemonic of @p direction (N/E/S/O).
 */
[[nodiscard]] std::string_view directionLetter(Direction direction);

/**
 * @brief One-tile movement vector for @p direction.
 */
[[nodiscard]] DirectionOffset directionOffset(Direction direction);

/**
 * @brief The direction facing the opposite way to @p direction.
 */
[[nodiscard]] Direction opposite(Direction direction);

/**
 * @brief The direction reached by turning @p direction 90 degrees clockwise
 *        (the AI `Right` command): N->E->S->W->N.
 */
[[nodiscard]] Direction turnRight(Direction direction);

/**
 * @brief The direction reached by turning @p direction 90 degrees
 *        counter-clockwise (the AI `Left` command): N->W->S->E->N.
 */
[[nodiscard]] Direction turnLeft(Direction direction);

/**
 * @brief Eject `eject: K` code received by a drone pushed off a tile.
 *
 * K is the direction the pushed drone comes from, expressed relative to its
 * own orientation with the subject's tile numbering (1 = front, then
 * counter-clockwise). A cardinal push therefore yields one of front (1),
 * left (3), back (5) or right (7).
 *
 * @param[in] pushDirection Direction the ejecting drone is facing (the push).
 * @param[in] victimFacing  Orientation of the drone being pushed.
 * @returns The K code to send to the pushed drone.
 */
[[nodiscard]] int ejectionCode(Direction pushDirection, Direction victimFacing);

}  // namespace zappy::world
