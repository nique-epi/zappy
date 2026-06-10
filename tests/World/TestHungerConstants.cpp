/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Tests that hunger constants match the Zappy subject rules
*/

#include <gtest/gtest.h>
#include "App/World/Player/HungerService.hpp"
#include "App/World/Player/Player.hpp"

using zappy::server::gameTicksPerFoodUnit;
using zappy::world::initialFoodUnits;

TEST(HungerConstants, OneFoodUnitMatchesSubjectRule) {
  /*
   * Given the subject rule "1 food unit = 126 time units"
   * When gameTicksPerFoodUnit is read
   * Then it equals 126
   */
  EXPECT_EQ(gameTicksPerFoodUnit, 126);
}

TEST(HungerConstants, InitialReserveLastsExactlyTheSubjectLifespan) {
  /*
   * Given the subject rule "starting food = 10 units of 126 time units each"
   * When the product of the two constants is computed
   * Then it equals the 1260 time units announced by the subject
   */
  EXPECT_EQ(initialFoodUnits * gameTicksPerFoodUnit, 1260);
}
