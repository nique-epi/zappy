/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Direction — cardinal orientation of a drone on the toroidal world
*/

#include "App/World/Player/Direction.hpp"
#include <array>

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

namespace {

constexpr int cardinalCount = 4;

/// `eject: K` codes. The subject numbers tiles 1 in front of the drone then
/// counter-clockwise; a cardinal push therefore reaches one of these sides.
constexpr int ejectFromFront = 1;
constexpr int ejectFromLeft = 3;
constexpr int ejectFromBehind = 5;
constexpr int ejectFromRight = 7;

/// Position of @p direction in clockwise order N, E, S, W (North being 0).
int cardinalIndex(Direction direction) {
  return static_cast<int>(direction) - static_cast<int>(Direction::North);
}

/// Number of 90-degree clockwise turns rotating @p fromDirection onto
/// @p toDirection.
int clockwiseQuarterTurns(Direction fromDirection, Direction toDirection) {
  const int delta = cardinalIndex(toDirection) - cardinalIndex(fromDirection);
  return (delta % cardinalCount + cardinalCount) % cardinalCount;
}

}  // namespace

int ejectionCode(Direction pushDirection, Direction victimFacing) {
  constexpr std::array<int, cardinalCount> codeBySourceTurns = {
      ejectFromFront, ejectFromRight, ejectFromBehind, ejectFromLeft};
  const Direction source = opposite(pushDirection);
  return codeBySourceTurns.at(clockwiseQuarterTurns(victimFacing, source));
}

}  // namespace zappy::world
