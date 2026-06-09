/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the resource loot service
*/

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include "App/Loot/LootService.hpp"
#include "App/Loot/ResourceDensity.hpp"
#include "App/Scheduler/ActionTiming.hpp"
#include "App/Scheduler/Exceptions/SchedulerException.hpp"
#include "App/Scheduler/Scheduler.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Tile/Tile.hpp"

using zappy::loot::LootService;
using zappy::loot::refillTimeUnits;
using zappy::loot::targetQuantity;
using zappy::server::actionDuration;
using zappy::server::InvalidFrequencyException;
using zappy::server::Scheduler;
using zappy::server::SchedulerException;
using zappy::world::allResourceTypes;
using zappy::world::Map;
using zappy::world::ResourceType;

namespace {
constexpr int frequency = 100;
constexpr std::uint_fast32_t seed = 42;

std::size_t totalOnMap(const Map &map, ResourceType type) {
  std::size_t total = 0;

  for (int row = 0; row < map.height(); ++row) {
    for (int column = 0; column < map.width(); ++column) {
      total += map.tileAt(column, row).quantityOf(type);
    }
  }
  return total;
}

void emptyMap(Map &map) {
  for (int row = 0; row < map.height(); ++row) {
    for (int column = 0; column < map.width(); ++column) {
      zappy::world::Tile &tile = map.tileAt(column, row);

      for (const ResourceType type : allResourceTypes()) {
        tile.collect(type, tile.quantityOf(type));
      }
    }
  }
}
}  // namespace

TEST(LootService, StocksEveryKindToTargetOnFreshMap) {
  Map map(10, 10);
  LootService loot(map, frequency, seed);

  loot.replenish();

  for (const ResourceType type : allResourceTypes()) {
    EXPECT_EQ(totalOnMap(map, type), targetQuantity(type, 10, 10));
  }
}

TEST(LootService, RejectsNonPositiveFrequencyAtConstruction) {
  Map map(10, 10);

  EXPECT_THROW(LootService(map, 0, seed), InvalidFrequencyException);
  EXPECT_THROW(LootService(map, -1, seed), InvalidFrequencyException);
  EXPECT_THROW(LootService(map, 0, seed), SchedulerException);
}

TEST(LootService, LeavesAtLeastOneOfEachKind) {
  Map map(3, 3);
  LootService loot(map, frequency, seed);

  loot.replenish();

  for (const ResourceType type : allResourceTypes()) {
    EXPECT_GE(totalOnMap(map, type), 1U);
  }
}

TEST(LootService, RefillTopsBackUpToTargetAfterDepletion) {
  Map map(10, 10);
  LootService loot(map, frequency, seed);
  loot.replenish();
  emptyMap(map);

  loot.replenish();

  EXPECT_EQ(totalOnMap(map, ResourceType::Food),
            targetQuantity(ResourceType::Food, 10, 10));
  EXPECT_EQ(totalOnMap(map, ResourceType::Thystame),
            targetQuantity(ResourceType::Thystame, 10, 10));
}

TEST(LootService, NeverRemovesSurplusAboveTarget) {
  Map map(10, 10);
  LootService loot(map, frequency, seed);
  loot.replenish();
  const std::size_t target = targetQuantity(ResourceType::Sibur, 10, 10);
  map.tileAt(0, 0).drop(ResourceType::Sibur, target + 7);
  const std::size_t before = totalOnMap(map, ResourceType::Sibur);

  loot.replenish();

  EXPECT_GT(before, target);
  EXPECT_EQ(totalOnMap(map, ResourceType::Sibur), before);
}

TEST(LootService, SameSeedYieldsSameDistribution) {
  Map first(10, 10);
  Map second(10, 10);

  LootService(first, frequency, seed).replenish();
  LootService(second, frequency, seed).replenish();

  for (int row = 0; row < 10; ++row) {
    for (int column = 0; column < 10; ++column) {
      for (const ResourceType type : allResourceTypes()) {
        EXPECT_EQ(first.tileAt(column, row).quantityOf(type),
                  second.tileAt(column, row).quantityOf(type));
      }
    }
  }
}

TEST(LootService, RefillPeriodFollowsTheTimeUnitFormula) {
  Map map(10, 10);
  LootService loot(map, frequency, seed);

  EXPECT_EQ(loot.refillPeriod(), actionDuration(refillTimeUnits, frequency));
}

TEST(LootService, StartArmsRecurringRefillTopsUpEachPeriod) {
  Map map(10, 10);
  LootService loot(map, frequency, seed);
  Scheduler scheduler;
  const Scheduler::TimePoint base{};
  const std::size_t foodTarget = targetQuantity(ResourceType::Food, 10, 10);

  loot.start(scheduler, base);

  EXPECT_EQ(totalOnMap(map, ResourceType::Food), foodTarget);
  EXPECT_EQ(scheduler.pendingCount(), 1U);

  emptyMap(map);
  scheduler.runDue(base + loot.refillPeriod());

  EXPECT_EQ(totalOnMap(map, ResourceType::Food), foodTarget);
  EXPECT_EQ(scheduler.pendingCount(), 1U);

  emptyMap(map);
  scheduler.runDue(base + loot.refillPeriod() + loot.refillPeriod());

  EXPECT_EQ(totalOnMap(map, ResourceType::Food), foodTarget);
  EXPECT_EQ(scheduler.pendingCount(), 1U);
}
