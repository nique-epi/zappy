/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for Player
*/

#include <gtest/gtest.h>
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"

using zappy::world::Direction;
using zappy::world::directionLetter;
using zappy::world::initialFoodUnits;
using zappy::world::Player;
using zappy::world::ResourceType;

TEST(Player, ConstructionSetsIdentityAndPreChargesFood) {
  const Player player(7, "red", 3, 4, Direction::East);

  EXPECT_EQ(player.id(), 7);
  EXPECT_EQ(player.teamName(), "red");
  EXPECT_EQ(player.x(), 3);
  EXPECT_EQ(player.y(), 4);
  EXPECT_EQ(player.direction(), Direction::East);
  EXPECT_EQ(player.level(), 1);
  EXPECT_EQ(player.inventory().quantityOf(ResourceType::Food),
            static_cast<std::size_t>(initialFoodUnits));
}

TEST(Player, MutatorsUpdateStateInPlace) {
  Player player(1, "red", 0, 0, Direction::North);

  player.setPosition(5, 6);
  player.setDirection(Direction::South);
  player.setLevel(3);

  EXPECT_EQ(player.x(), 5);
  EXPECT_EQ(player.y(), 6);
  EXPECT_EQ(player.direction(), Direction::South);
  EXPECT_EQ(player.level(), 3);
}

TEST(Direction, LetterMatchesProtocolMnemonics) {
  EXPECT_EQ(directionLetter(Direction::North), "N");
  EXPECT_EQ(directionLetter(Direction::East), "E");
  EXPECT_EQ(directionLetter(Direction::South), "S");
  EXPECT_EQ(directionLetter(Direction::West), "O");
}
