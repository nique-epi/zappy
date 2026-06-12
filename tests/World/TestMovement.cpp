/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for Movement (moveForward)
*/

#include <gtest/gtest.h>
#include <vector>
#include "App/World/Map/Map.hpp"
#include "App/World/Movement.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Tile/Tile.hpp"

using zappy::world::Direction;
using zappy::world::Map;
using zappy::world::moveForward;
using zappy::world::Player;

TEST(Movement, MoveForwardStepsAlongOrientation) {
  /*
   * Given a drone facing East on an interior tile, recorded on its tile
   * When it moves forward
   * Then its column increases by one and the tile bookkeeping follows it
   */
  Map map(5, 5);
  Player player(1, "red", 2, 2, Direction::East);
  map.tileAt(2, 2).addPlayer(player.id());

  moveForward(player, map);

  EXPECT_EQ(player.x(), 3);
  EXPECT_EQ(player.y(), 2);
  EXPECT_TRUE(map.tileAt(2, 2).players().empty());
  EXPECT_EQ(map.tileAt(3, 2).players(), std::vector<int>{1});
}

TEST(Movement, MoveForwardWrapsAcrossTheNorthEdge) {
  /*
   * Given a drone facing North on the top row of a 3x3 world
   * When it moves forward off the edge
   * Then it re-enters on the bottom row (toroidal wrapping)
   */
  Map map(3, 3);
  Player player(7, "red", 0, 0, Direction::North);
  map.tileAt(0, 0).addPlayer(player.id());

  moveForward(player, map);

  EXPECT_EQ(player.x(), 0);
  EXPECT_EQ(player.y(), 2);
  EXPECT_TRUE(map.tileAt(0, 0).players().empty());
  EXPECT_EQ(map.tileAt(0, 2).players(), std::vector<int>{7});
}
