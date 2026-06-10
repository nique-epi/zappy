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

DirectionOffset directionOffset(Direction direction) {
  switch (direction) {
    case Direction::North:
      return {.dx = 0, .dy = -1};
    case Direction::East:
      return {.dx = 1, .dy = 0};
    case Direction::South:
      return {.dx = 0, .dy = 1};
    case Direction::West:
      return {.dx = -1, .dy = 0};
  }
  return {.dx = 0, .dy = 0};
}

Direction opposite(Direction direction) {
  switch (direction) {
    case Direction::North:
      return Direction::South;
    case Direction::East:
      return Direction::West;
    case Direction::South:
      return Direction::North;
    case Direction::West:
      return Direction::East;
  }
  return direction;
}

int ejectionCode(Direction pushDirection, Direction victimFacing) {
  constexpr int cardinalCount = 4;
  const int comingFrom = static_cast<int>(opposite(pushDirection)) - 1;
  const int facing = static_cast<int>(victimFacing) - 1;
  const int clockwiseSteps =
      ((comingFrom - facing) % cardinalCount + cardinalCount) % cardinalCount;
  switch (clockwiseSteps) {
    case 0:
      return 1;
    case 1:
      return 7;
    case 2:
      return 5;
    default:
      return 3;
  }
}

}  // namespace zappy::world
