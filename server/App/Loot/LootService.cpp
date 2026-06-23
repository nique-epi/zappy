/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** LootService — distributes and periodically replenishes world resources
*/

#include "App/Loot/LootService.hpp"
#include "App/Loot/ResourceDensity.hpp"
#include "App/Scheduler/ActionTiming.hpp"
#include "App/Scheduler/Exceptions/SchedulerException.hpp"
#include "Net/Gui/GuiEventBroadcaster.hpp"

namespace zappy::loot {

LootService::LootService(world::Map &map, server::GuiEventBroadcaster &gui,
                         int frequency, std::uint_fast32_t seed)
    : map_(map), gui_(gui), frequency_(frequency), rng_(seed) {
  if (frequency <= 0) {
    throw server::InvalidFrequencyException(frequency);
  }
}

void LootService::replenish() {
  std::set<std::pair<int, int>> changed;

  for (const world::ResourceType type : world::allResourceTypes()) {
    const std::size_t target =
        targetQuantity(type, map_.width(), map_.height());
    const std::size_t current = currentTotal(type);

    if (current < target) {
      scatter(type, target - current, changed);
    }
  }
  for (const auto &[column, row] : changed) {
    gui_.tileChanged(column, row, map_.tileAt(column, row));
  }
}

std::chrono::nanoseconds LootService::refillPeriod() const {
  return server::actionDuration(refillTimeUnits, frequency_);
}

void LootService::start(server::Scheduler &scheduler,
                        server::Scheduler::TimePoint now) {
  replenish();
  scheduleNext(scheduler, now + refillPeriod());
}

void LootService::scheduleNext(server::Scheduler &scheduler,
                               server::Scheduler::TimePoint deadline) {
  scheduler.scheduleAt(deadline, [this, &scheduler, deadline] {
    replenish();
    scheduleNext(scheduler, deadline + refillPeriod());
  });
}

std::size_t LootService::currentTotal(world::ResourceType type) const {
  std::size_t total = 0;

  for (int row = 0; row < map_.height(); ++row) {
    for (int column = 0; column < map_.width(); ++column) {
      total += map_.tileAt(column, row).quantityOf(type);
    }
  }
  return total;
}

void LootService::scatter(world::ResourceType type, std::size_t amount,
                          std::set<std::pair<int, int>> &changed) {
  std::uniform_int_distribution<int> columnPick(0, map_.width() - 1);
  std::uniform_int_distribution<int> rowPick(0, map_.height() - 1);

  for (std::size_t placed = 0; placed < amount; ++placed) {
    const int column = columnPick(rng_);
    const int row = rowPick(rng_);
    map_.tileAt(column, row).drop(type);
    changed.insert({column, row});
  }
}

}  // namespace zappy::loot
