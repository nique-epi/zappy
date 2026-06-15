/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the elevation requirements table
*/

#include <gtest/gtest.h>
#include "App/World/Exceptions/WorldException.hpp"
#include "App/World/Incantation/ElevationRequirements.hpp"
#include "App/World/Resources/ResourceType.hpp"

using zappy::world::ElevationRequirement;
using zappy::world::elevationRequirement;
using zappy::world::InvalidElevationLevelException;
using zappy::world::ResourceType;

TEST(ElevationRequirements, FirstStepNeedsOnePlayerAndOneLinemate) {
  /*
   * Given the elevation step from level 1
   * When the requirement is looked up
   * Then it asks for one player and a single linemate
   */
  const ElevationRequirement& need = elevationRequirement(1);

  EXPECT_EQ(need.playersRequired, 1U);
  EXPECT_EQ(need.stones[static_cast<std::size_t>(ResourceType::Linemate)], 1U);
  EXPECT_EQ(need.stones[static_cast<std::size_t>(ResourceType::Deraumere)], 0U);
}

TEST(ElevationRequirements, LastStepMatchesTheSubjectTable) {
  /*
   * Given the elevation step from level 7
   * When the requirement is looked up
   * Then it matches the subject's 7->8 row exactly
   */
  const ElevationRequirement& need = elevationRequirement(7);

  EXPECT_EQ(need.playersRequired, 6U);
  EXPECT_EQ(need.stones[static_cast<std::size_t>(ResourceType::Linemate)], 2U);
  EXPECT_EQ(need.stones[static_cast<std::size_t>(ResourceType::Deraumere)], 2U);
  EXPECT_EQ(need.stones[static_cast<std::size_t>(ResourceType::Sibur)], 2U);
  EXPECT_EQ(need.stones[static_cast<std::size_t>(ResourceType::Mendiane)], 2U);
  EXPECT_EQ(need.stones[static_cast<std::size_t>(ResourceType::Phiras)], 2U);
  EXPECT_EQ(need.stones[static_cast<std::size_t>(ResourceType::Thystame)], 1U);
}

TEST(ElevationRequirements, FoodIsNeverPartOfARitual) {
  /*
   * Given every elevation step
   * When each requirement is inspected
   * Then no step ever consumes food
   */
  for (int fromLevel = 1; fromLevel <= 7; ++fromLevel) {
    const ElevationRequirement& need = elevationRequirement(fromLevel);
    EXPECT_EQ(need.stones[static_cast<std::size_t>(ResourceType::Food)], 0U);
  }
}

TEST(ElevationRequirements, LevelOutsideTheElevableRangeThrows) {
  /*
   * Given levels below 1 and at the cap
   * When the requirement is looked up
   * Then an InvalidElevationLevelException is raised
   */
  EXPECT_THROW((void)elevationRequirement(0), InvalidElevationLevelException);
  EXPECT_THROW((void)elevationRequirement(8), InvalidElevationLevelException);
}
