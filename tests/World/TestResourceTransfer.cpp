/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for ResourceTransfer (takeResource, dropResource)
*/

#include <gtest/gtest.h>
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/ResourceTransfer.hpp"
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Tile/Tile.hpp"

using zappy::world::Direction;
using zappy::world::dropResource;
using zappy::world::Map;
using zappy::world::Player;
using zappy::world::ResourceType;
using zappy::world::takeResource;

TEST(ResourceTransfer, TakeResourceTransfersWhenPresent) {
  /*
   * Given a tile holding one linemate under a drone
   * When the drone takes a linemate
   * Then the unit leaves the tile and enters the inventory
   */
  Map map(3, 3);
  Player player(1, "red", 1, 1, Direction::North);
  map.tileAt(1, 1).drop(ResourceType::Linemate);

  EXPECT_TRUE(takeResource(player, map.tileAt(1, 1), ResourceType::Linemate));
  EXPECT_EQ(map.tileAt(1, 1).quantityOf(ResourceType::Linemate), 0U);
  EXPECT_EQ(player.inventory().quantityOf(ResourceType::Linemate), 1U);
}

TEST(ResourceTransfer, TakeResourceFailsWhenTileIsEmpty) {
  /*
   * Given a tile with no sibur under a drone
   * When the drone tries to take a sibur
   * Then nothing is transferred and the action reports failure
   */
  Map map(3, 3);
  Player player(1, "red", 1, 1, Direction::North);

  EXPECT_FALSE(takeResource(player, map.tileAt(1, 1), ResourceType::Sibur));
  EXPECT_EQ(player.inventory().quantityOf(ResourceType::Sibur), 0U);
}

TEST(ResourceTransfer, DropResourcePlacesCarriedUnitOnTile) {
  /*
   * Given a drone carrying one deraumere
   * When it sets a deraumere down
   * Then the unit leaves the inventory and lands on the tile
   */
  Map map(3, 3);
  Player player(1, "red", 1, 1, Direction::North);
  player.inventory().add(ResourceType::Deraumere);

  EXPECT_TRUE(dropResource(player, map.tileAt(1, 1), ResourceType::Deraumere));
  EXPECT_EQ(player.inventory().quantityOf(ResourceType::Deraumere), 0U);
  EXPECT_EQ(map.tileAt(1, 1).quantityOf(ResourceType::Deraumere), 1U);
}

TEST(ResourceTransfer, DropResourceFailsWhenNotCarried) {
  /*
   * Given a drone carrying no phiras
   * When it tries to set a phiras down
   * Then nothing is transferred and the action reports failure
   */
  Map map(3, 3);
  Player player(1, "red", 1, 1, Direction::North);

  EXPECT_FALSE(dropResource(player, map.tileAt(1, 1), ResourceType::Phiras));
  EXPECT_EQ(map.tileAt(1, 1).quantityOf(ResourceType::Phiras), 0U);
}
