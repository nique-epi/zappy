/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Direction — cardinal orientation of a drone on the toroidal world
*/

#include "App/World/Player/Direction.hpp"

namespace zappy::world {

std::string_view directionLetter(Direction direction) {
  switch (direction) {
    case Direction::North:
      return "N";
    case Direction::East:
      return "E";
    case Direction::South:
      return "S";
    case Direction::West:
      return "O";
  }
  return "?";
}

}  // namespace zappy::world
