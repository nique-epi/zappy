/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** LootService — distributes and periodically replenishes world resources
*/

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <random>
#include "App/Scheduler/Scheduler.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Resources/ResourceType.hpp"

namespace zappy::loot {

/**
 * @brief Owns the resource-spawning rules of the world floor: it stocks the
 *        map at start-up and tops it back up to target every @ref
 *        refillTimeUnits game-time units.
 *
 * The service never removes resources; each replenishment only adds the
 * shortfall between the current stock and the target given by @ref
 * targetQuantity, so player-deposited surplus is preserved and at least one
 * unit of every kind is always present. Units are scattered uniformly at
 * random, which spreads them evenly across the toroidal surface.
 */
class LootService {
 public:
  /**
   * @brief Build a service bound to @p map.
   * @param[in] map       The world surface to stock; must outlive the service.
   * @param[in] frequency Reciprocal time unit `f` (> 0) setting the refill
   *                      period (`refillTimeUnits / f` seconds).
   * @param[in] seed      Seed of the placement random engine; a fixed seed
   *                      yields a reproducible distribution.
   * @throws server::InvalidFrequencyException if @p frequency is not strictly
   *         positive, so the broken contract fails fast at construction rather
   *         than when the refill period is later computed.
   */
  LootService(world::Map &map, int frequency, std::uint_fast32_t seed);

  /**
   * @brief Top every resource kind back up to its target stock on the map.
   *
   * Used both for the start-up distribution (on an empty map the stock reaches
   * the target exactly) and for each periodic refill.
   */
  void replenish();

  /**
   * @brief Wall-clock delay between two replenishments.
   * @returns `refillTimeUnits / frequency` seconds as a nanosecond duration.
   */
  [[nodiscard]] std::chrono::nanoseconds refillPeriod() const;

  /**
   * @brief Perform the start-up distribution and arm the recurring refill.
   * @param[in] scheduler Queue the recurring refill is registered on.
   * @param[in] now       Current time reference; the first refill is due at
   *                      `now + refillPeriod()`.
   */
  void start(server::Scheduler &scheduler, server::Scheduler::TimePoint now);

 private:
  void scheduleNext(server::Scheduler &scheduler,
                    server::Scheduler::TimePoint deadline);
  [[nodiscard]] std::size_t currentTotal(world::ResourceType type) const;
  void scatter(world::ResourceType type, std::size_t amount);

  world::Map &map_;
  int frequency_;
  std::mt19937 rng_;
};

}  // namespace zappy::loot
