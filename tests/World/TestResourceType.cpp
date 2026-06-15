/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the ResourceType enumeration and its helpers
*/

#include <gtest/gtest.h>
#include <cstddef>
#include "App/World/Resources/ResourceType.hpp"

using zappy::world::allResourceTypes;
using zappy::world::resourceFromName;
using zappy::world::resourceName;
using zappy::world::ResourceType;
using zappy::world::resourceTypeCount;

TEST(ResourceType, CountsFoodPlusSixMinerals) {
  EXPECT_EQ(resourceTypeCount, 7U);
  EXPECT_EQ(allResourceTypes().size(), resourceTypeCount);
}

TEST(ResourceType, OrderMatchesGuiProtocolFields) {
  const auto& types = allResourceTypes();

  EXPECT_EQ(types[0], ResourceType::Food);
  EXPECT_EQ(types[1], ResourceType::Linemate);
  EXPECT_EQ(types[2], ResourceType::Deraumere);
  EXPECT_EQ(types[3], ResourceType::Sibur);
  EXPECT_EQ(types[4], ResourceType::Mendiane);
  EXPECT_EQ(types[5], ResourceType::Phiras);
  EXPECT_EQ(types[6], ResourceType::Thystame);
}

TEST(ResourceType, UnderlyingIndexMatchesProtocolPosition) {
  EXPECT_EQ(static_cast<std::size_t>(ResourceType::Food), 0U);
  EXPECT_EQ(static_cast<std::size_t>(ResourceType::Thystame), 6U);
}

TEST(ResourceType, NamesUseProtocolTokens) {
  EXPECT_EQ(resourceName(ResourceType::Food), "food");
  EXPECT_EQ(resourceName(ResourceType::Linemate), "linemate");
  EXPECT_EQ(resourceName(ResourceType::Deraumere), "deraumere");
  EXPECT_EQ(resourceName(ResourceType::Sibur), "sibur");
  EXPECT_EQ(resourceName(ResourceType::Mendiane), "mendiane");
  EXPECT_EQ(resourceName(ResourceType::Phiras), "phiras");
  EXPECT_EQ(resourceName(ResourceType::Thystame), "thystame");
}

TEST(ResourceType, ResourceFromNameResolvesEveryToken) {
  /*
   * Given each protocol token a name round-trips from
   * When resourceFromName is queried
   * Then it returns the kind that resourceName maps back to the token
   */
  for (const ResourceType type : allResourceTypes()) {
    EXPECT_EQ(resourceFromName(resourceName(type)), type);
  }
}

TEST(ResourceType, ResourceFromNameRejectsUnknownToken) {
  /*
   * Given a token that names no resource
   * When resourceFromName is queried
   * Then it returns no kind
   */
  EXPECT_FALSE(resourceFromName("wood").has_value());
  EXPECT_FALSE(resourceFromName("").has_value());
  EXPECT_FALSE(resourceFromName("Food").has_value());
}
