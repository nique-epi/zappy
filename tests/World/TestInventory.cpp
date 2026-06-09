/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for Inventory
*/

#include <gtest/gtest.h>
#include "App/World/Player/Inventory.hpp"

using zappy::world::Inventory;
using zappy::world::ResourceType;

TEST(Inventory, StartsEmpty) {
  /*
   * Given a freshly constructed inventory
   * When no resource has been added
   * Then every kind reports a zero count
   */
  const Inventory inventory;

  EXPECT_EQ(inventory.quantityOf(ResourceType::Food), 0U);
  EXPECT_EQ(inventory.quantityOf(ResourceType::Thystame), 0U);
}

TEST(Inventory, AddIncrementsTheKindOnly) {
  /*
   * Given an inventory accumulating two different resource kinds
   * When add is called for each kind
   * Then only those kinds are credited; unrelated kinds remain at zero
   */
  Inventory inventory;

  inventory.add(ResourceType::Food, 4);
  inventory.add(ResourceType::Linemate);

  EXPECT_EQ(inventory.quantityOf(ResourceType::Food), 4U);
  EXPECT_EQ(inventory.quantityOf(ResourceType::Linemate), 1U);
  EXPECT_EQ(inventory.quantityOf(ResourceType::Sibur), 0U);
}

TEST(Inventory, ConsumeRemovesAndReportsSuccess) {
  /*
   * Given an inventory holding 3 units of Food
   * When 2 units are consumed
   * Then consume returns true and 1 unit remains
   */
  Inventory inventory;
  inventory.add(ResourceType::Food, 3);

  EXPECT_TRUE(inventory.consume(ResourceType::Food, 2));
  EXPECT_EQ(inventory.quantityOf(ResourceType::Food), 1U);
}

TEST(Inventory, ConsumeRefusesToOverdraw) {
  /*
   * Given an inventory holding 2 units of Food
   * When more than the stock is asked
   * Then consume returns false and nothing is removed
   */
  Inventory inventory;
  inventory.add(ResourceType::Food, 2);

  EXPECT_FALSE(inventory.consume(ResourceType::Food, 5));
  EXPECT_EQ(inventory.quantityOf(ResourceType::Food), 2U);
}
