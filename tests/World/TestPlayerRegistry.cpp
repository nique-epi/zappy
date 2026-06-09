/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for PlayerRegistry
*/

#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include "App/World/Map/Map.hpp"
#include "App/World/Player/PlayerRegistry.hpp"

using zappy::world::Direction;
using zappy::world::Map;
using zappy::world::Player;
using zappy::world::PlayerRegistry;

TEST(PlayerRegistry, StartsEmpty) {
  const PlayerRegistry registry;

  EXPECT_EQ(registry.size(), 0U);
  EXPECT_FALSE(registry.contains(1));
  EXPECT_EQ(registry.find(1), nullptr);
}

TEST(PlayerRegistry, SpawnPushesPlayerOntoTileAndAssignsIncrementingId) {
  PlayerRegistry registry;
  Map map(5, 5);

  const Player& first = registry.spawn("red", 1, 2, Direction::North, map);
  const Player& second = registry.spawn("blue", 3, 4, Direction::South, map);

  EXPECT_NE(first.id(), second.id());
  EXPECT_EQ(registry.size(), 2U);
  EXPECT_TRUE(registry.contains(first.id()));
  ASSERT_EQ(map.tileAt(1, 2).players().size(), 1U);
  EXPECT_EQ(map.tileAt(1, 2).players().front(), first.id());
  EXPECT_EQ(map.tileAt(3, 4).players().front(), second.id());
}

TEST(PlayerRegistry, RemoveDropsPlayerAndFreesTile) {
  PlayerRegistry registry;
  Map map(5, 5);
  const int playerId = registry.spawn("red", 1, 2, Direction::East, map).id();

  EXPECT_TRUE(registry.remove(playerId, map));

  EXPECT_EQ(registry.size(), 0U);
  EXPECT_FALSE(registry.contains(playerId));
  EXPECT_TRUE(map.tileAt(1, 2).players().empty());
}

TEST(PlayerRegistry, RemoveOfUnknownIdIsANoOp) {
  PlayerRegistry registry;
  Map map(5, 5);

  EXPECT_FALSE(registry.remove(99, map));
}

TEST(PlayerRegistry, IdsReflectsInsertionOrder) {
  PlayerRegistry registry;
  Map map(5, 5);
  const int firstId = registry.spawn("red", 0, 0, Direction::North, map).id();
  const int secondId = registry.spawn("red", 1, 1, Direction::East, map).id();
  const int thirdId = registry.spawn("red", 2, 2, Direction::South, map).id();

  const std::vector<int> ids = registry.ids();

  ASSERT_EQ(ids.size(), 3U);
  EXPECT_EQ(ids[0], firstId);
  EXPECT_EQ(ids[1], secondId);
  EXPECT_EQ(ids[2], thirdId);
}

TEST(PlayerRegistry, RandomDirectionStaysInTheFourCardinalSet) {
  std::mt19937 rng(7);
  for (int i = 0; i < 32; ++i) {
    const Direction direction = PlayerRegistry::randomDirection(rng);
    EXPECT_TRUE(direction == Direction::North || direction == Direction::East ||
                direction == Direction::South || direction == Direction::West);
  }
}
