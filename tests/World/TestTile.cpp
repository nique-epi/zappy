/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the Tile content model
*/

#include <gtest/gtest.h>
#include <vector>
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Tile/Tile.hpp"

using zappy::world::ResourceType;
using zappy::world::Tile;

TEST(Tile, StartsEmpty) {
  const Tile tile;

  EXPECT_EQ(tile.quantityOf(ResourceType::Food), 0U);
  EXPECT_EQ(tile.quantityOf(ResourceType::Thystame), 0U);
  EXPECT_TRUE(tile.players().empty());
  EXPECT_TRUE(tile.eggs().empty());
}

TEST(Tile, DropAddsToTheMatchingStockOnly) {
  Tile tile;

  tile.drop(ResourceType::Linemate, 3);
  tile.drop(ResourceType::Linemate);

  EXPECT_EQ(tile.quantityOf(ResourceType::Linemate), 4U);
  EXPECT_EQ(tile.quantityOf(ResourceType::Sibur), 0U);
}

TEST(Tile, CollectRemovesWhenEnoughIsAvailable) {
  Tile tile;
  tile.drop(ResourceType::Sibur, 2);

  EXPECT_TRUE(tile.collect(ResourceType::Sibur));
  EXPECT_EQ(tile.quantityOf(ResourceType::Sibur), 1U);
  EXPECT_TRUE(tile.collect(ResourceType::Sibur));
  EXPECT_EQ(tile.quantityOf(ResourceType::Sibur), 0U);
}

TEST(Tile, CollectFailsAndLeavesStockUntouchedWhenInsufficient) {
  Tile tile;
  tile.drop(ResourceType::Mendiane, 1);

  EXPECT_FALSE(tile.collect(ResourceType::Mendiane, 2));
  EXPECT_EQ(tile.quantityOf(ResourceType::Mendiane), 1U);
  EXPECT_FALSE(tile.collect(ResourceType::Phiras));
}

TEST(Tile, TracksPlayersById) {
  Tile tile;

  tile.addPlayer(7);
  tile.addPlayer(42);
  EXPECT_EQ(tile.players(), (std::vector<int>{7, 42}));

  EXPECT_TRUE(tile.removePlayer(7));
  EXPECT_EQ(tile.players(), std::vector<int>{42});
  EXPECT_FALSE(tile.removePlayer(7));
}

TEST(Tile, TracksEggsById) {
  Tile tile;

  tile.addEgg(1);
  tile.addEgg(2);
  EXPECT_EQ(tile.eggs(), (std::vector<int>{1, 2}));

  EXPECT_TRUE(tile.removeEgg(2));
  EXPECT_EQ(tile.eggs(), std::vector<int>{1});
  EXPECT_FALSE(tile.removeEgg(99));
}
