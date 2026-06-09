/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for TeamRegistry
*/

#include <gtest/gtest.h>
#include <random>
#include "App/World/Exceptions/WorldException.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Team/TeamRegistry.hpp"

using zappy::world::Egg;
using zappy::world::Map;
using zappy::world::NoFreeSlotException;
using zappy::world::TeamRegistry;
using zappy::world::UnknownTeamException;

namespace {
std::mt19937 deterministicRng() { return std::mt19937(42); }
}  // namespace

TEST(TeamRegistry, ContainsRegisteredTeams) {
  const TeamRegistry registry({"red", "blue"});

  EXPECT_TRUE(registry.contains("red"));
  EXPECT_TRUE(registry.contains("blue"));
  EXPECT_FALSE(registry.contains("green"));
}

TEST(TeamRegistry, FreeSlotsOfUnknownTeamThrows) {
  const TeamRegistry registry({"red"});

  EXPECT_THROW((void)registry.freeSlots("ghost"), UnknownTeamException);
}

TEST(TeamRegistry, SeedInitialEggsPopulatesEveryTeamAndTile) {
  TeamRegistry registry({"red", "blue"});
  Map map(10, 10);
  std::mt19937 rng = deterministicRng();

  registry.seedInitialEggs(map, 4, rng);

  EXPECT_EQ(registry.freeSlots("red"), 4U);
  EXPECT_EQ(registry.freeSlots("blue"), 4U);
  std::size_t totalEggsOnMap = 0;
  for (int y = 0; y < map.height(); ++y) {
    for (int x = 0; x < map.width(); ++x) {
      totalEggsOnMap += map.tileAt(x, y).eggs().size();
    }
  }
  EXPECT_EQ(totalEggsOnMap, 8U);
}

TEST(TeamRegistry, HatchRemovesEggFromTeamAndTile) {
  TeamRegistry registry({"red"});
  Map map(10, 10);
  std::mt19937 rng = deterministicRng();
  registry.seedInitialEggs(map, 3, rng);

  const Egg hatched = registry.hatch("red", map, rng);

  EXPECT_EQ(hatched.teamName, "red");
  EXPECT_EQ(registry.freeSlots("red"), 2U);
  const auto& tileEggs = map.tileAt(hatched.x, hatched.y).eggs();
  EXPECT_EQ(std::find(tileEggs.begin(), tileEggs.end(), hatched.id),
            tileEggs.end());
}

TEST(TeamRegistry, HatchOnEmptyTeamThrows) {
  TeamRegistry registry({"red"});
  Map map(5, 5);
  std::mt19937 rng = deterministicRng();

  EXPECT_THROW((void)registry.hatch("red", map, rng), NoFreeSlotException);
}

TEST(TeamRegistry, HatchOnUnknownTeamThrows) {
  TeamRegistry registry({"red"});
  Map map(5, 5);
  std::mt19937 rng = deterministicRng();

  EXPECT_THROW((void)registry.hatch("ghost", map, rng), UnknownTeamException);
}

TEST(TeamRegistry, LayAddsEggToTeamAndTile) {
  TeamRegistry registry({"red"});
  Map map(5, 5);

  const Egg laid = registry.lay("red", map, 2, 3);

  EXPECT_EQ(registry.freeSlots("red"), 1U);
  EXPECT_EQ(laid.x, 2);
  EXPECT_EQ(laid.y, 3);
  const auto& tileEggs = map.tileAt(2, 3).eggs();
  ASSERT_EQ(tileEggs.size(), 1U);
  EXPECT_EQ(tileEggs.front(), laid.id);
}

TEST(TeamRegistry, LayOnUnknownTeamThrows) {
  TeamRegistry registry({"red"});
  Map map(5, 5);

  EXPECT_THROW((void)registry.lay("ghost", map, 0, 0), UnknownTeamException);
}
