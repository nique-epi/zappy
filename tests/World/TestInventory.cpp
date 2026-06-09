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
  const Inventory inventory;

  EXPECT_EQ(inventory.quantityOf(ResourceType::Food), 0U);
  EXPECT_EQ(inventory.quantityOf(ResourceType::Thystame), 0U);
}

TEST(Inventory, AddIncrementsTheKindOnly) {
  Inventory inventory;

  inventory.add(ResourceType::Food, 4);
  inventory.add(ResourceType::Linemate);

  EXPECT_EQ(inventory.quantityOf(ResourceType::Food), 4U);
  EXPECT_EQ(inventory.quantityOf(ResourceType::Linemate), 1U);
  EXPECT_EQ(inventory.quantityOf(ResourceType::Sibur), 0U);
}

TEST(Inventory, ConsumeRemovesAndReportsSuccess) {
  Inventory inventory;
  inventory.add(ResourceType::Food, 3);

  EXPECT_TRUE(inventory.consume(ResourceType::Food, 2));
  EXPECT_EQ(inventory.quantityOf(ResourceType::Food), 1U);
}

TEST(Inventory, ConsumeRefusesToOverdraw) {
  Inventory inventory;
  inventory.add(ResourceType::Food, 2);

  EXPECT_FALSE(inventory.consume(ResourceType::Food, 5));
  EXPECT_EQ(inventory.quantityOf(ResourceType::Food), 2U);
}
