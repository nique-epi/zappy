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
  EXPECT_EQ(gameTicksPerFoodUnit, 126);
}

TEST(HungerConstants, InitialReserveLastsExactlyTheSubjectLifespan) {
  EXPECT_EQ(initialFoodUnits * gameTicksPerFoodUnit, 1260);
}
