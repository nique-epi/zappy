/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for Team
*/

#include <gtest/gtest.h>
#include "App/World/Team/Team.hpp"

using zappy::world::Egg;
using zappy::world::Team;

TEST(Team, KeepsConstructionName) {
  const Team team("red");

  EXPECT_EQ(team.name(), "red");
  EXPECT_EQ(team.freeSlots(), 0U);
  EXPECT_TRUE(team.empty());
}

TEST(Team, AddEggGrowsFreeSlots) {
  Team team("red");

  team.addEgg(Egg{1, "red", 3, 4});
  team.addEgg(Egg{2, "red", 0, 0});

  EXPECT_EQ(team.freeSlots(), 2U);
  EXPECT_FALSE(team.empty());
}

TEST(Team, RemoveEggReturnsItAndShrinks) {
  Team team("red");
  team.addEgg(Egg{1, "red", 3, 4});
  team.addEgg(Egg{2, "red", 7, 1});

  const Egg removed = team.removeEggAt(0);

  EXPECT_EQ(removed.id, 1);
  EXPECT_EQ(removed.x, 3);
  EXPECT_EQ(removed.y, 4);
  ASSERT_EQ(team.freeSlots(), 1U);
  EXPECT_EQ(team.eggs().front().id, 2);
}

TEST(Team, RemoveEggOutOfRangeThrows) {
  Team team("red");

  EXPECT_THROW(team.removeEggAt(0), std::out_of_range);
}
