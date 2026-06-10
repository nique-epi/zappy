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
  /*
   * Given a freshly built team
   * When no egg has been pushed yet
   * Then it carries the construction name with zero free slots
   */
  const Team team("red");

  EXPECT_EQ(team.name(), "red");
  EXPECT_EQ(team.freeSlots(), 0U);
  EXPECT_TRUE(team.empty());
}

TEST(Team, AddEggGrowsFreeSlots) {
  /*
   * Given a team with no egg
   * When two eggs are appended
   * Then the free-slot count reflects the new size
   */
  Team team("red");

  team.addEgg(Egg{1, "red", 3, 4});
  team.addEgg(Egg{2, "red", 0, 0});

  EXPECT_EQ(team.freeSlots(), 2U);
  EXPECT_FALSE(team.empty());
}

TEST(Team, RemoveEggReturnsItAndShrinks) {
  /*
   * Given a team holding two eggs
   * When the first one is removed
   * Then the removed egg is returned and the team shrinks
   */
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
  /*
   * Given an empty team
   * When removeEggAt is called with any index
   * Then it throws std::out_of_range
   */
  Team team("red");

  EXPECT_THROW(team.removeEggAt(0), std::out_of_range);
}

TEST(Team, RemoveEggByIdDropsTheMatchingSlot) {
  /*
   * Given a team holding two eggs
   * When removeEggById targets the second one
   * Then it reports success, shrinks, and keeps the other egg
   */
  Team team("red");
  team.addEgg(Egg{1, "red", 3, 4});
  team.addEgg(Egg{2, "red", 7, 1});

  EXPECT_TRUE(team.removeEggById(2));
  ASSERT_EQ(team.freeSlots(), 1U);
  EXPECT_EQ(team.eggs().front().id, 1);
}

TEST(Team, RemoveEggByIdReturnsFalseForUnknownId) {
  /*
   * Given a team holding one egg
   * When removeEggById targets an absent id
   * Then it reports failure and leaves the pool untouched
   */
  Team team("red");
  team.addEgg(Egg{1, "red", 3, 4});

  EXPECT_FALSE(team.removeEggById(99));
  EXPECT_EQ(team.freeSlots(), 1U);
}
