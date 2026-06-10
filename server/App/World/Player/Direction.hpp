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
 * @brief Single-letter mnemonic of @p direction (N/E/S/O).
 */
[[nodiscard]] std::string_view directionLetter(Direction direction);

}  // namespace zappy::world
