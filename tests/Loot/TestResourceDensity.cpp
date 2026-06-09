/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the resource-density target formula
*/

#include <gtest/gtest.h>
#include "App/Loot/ResourceDensity.hpp"
#include "App/World/Resources/ResourceType.hpp"

using zappy::loot::targetQuantity;
using zappy::world::allResourceTypes;
using zappy::world::ResourceType;

TEST(ResourceDensity, MatchesSubjectExampleOnTenByTen) {
  EXPECT_EQ(targetQuantity(ResourceType::Food, 10, 10), 50U);
  EXPECT_EQ(targetQuantity(ResourceType::Thystame, 10, 10), 5U);
}

TEST(ResourceDensity, AppliesEveryDensityOnTenByTen) {
  EXPECT_EQ(targetQuantity(ResourceType::Food, 10, 10), 50U);
  EXPECT_EQ(targetQuantity(ResourceType::Linemate, 10, 10), 30U);
  EXPECT_EQ(targetQuantity(ResourceType::Deraumere, 10, 10), 15U);
  EXPECT_EQ(targetQuantity(ResourceType::Sibur, 10, 10), 10U);
  EXPECT_EQ(targetQuantity(ResourceType::Mendiane, 10, 10), 10U);
  EXPECT_EQ(targetQuantity(ResourceType::Phiras, 10, 10), 8U);
  EXPECT_EQ(targetQuantity(ResourceType::Thystame, 10, 10), 5U);
}

TEST(ResourceDensity, GuaranteesAtLeastOneOfEachOnTinyMap) {
  for (const ResourceType type : allResourceTypes()) {
    EXPECT_GE(targetQuantity(type, 1, 1), 1U);
  }
}

TEST(ResourceDensity, FloorsTheFormulaInIntegerArithmetic) {
  EXPECT_EQ(targetQuantity(ResourceType::Linemate, 7, 1), 2U);
  EXPECT_EQ(targetQuantity(ResourceType::Thystame, 7, 1), 1U);
}

TEST(ResourceDensity, ScalesWithMapArea) {
  EXPECT_EQ(targetQuantity(ResourceType::Food, 20, 20), 200U);
  EXPECT_EQ(targetQuantity(ResourceType::Thystame, 20, 20), 20U);
}
