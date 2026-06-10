/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Inventory — per-resource stock carried by a single player
*/

#pragma once

#include <array>
#include <cstddef>
#include "App/World/Resources/ResourceType.hpp"

namespace zappy::world {

/**
 * @brief A drone's personal stock of every resource kind.
 *
 * Stored as per-kind counts so two items of the same kind remain
 * indistinguishable, mirroring the rules of the world floor. Used by the AI
 * `Inventory` command, the `Take` / `Set` actions and as the food reserve
 * that drives the hunger / death loop.
 */
class Inventory {
 public:
  /// Number of units of @p type currently carried.
  [[nodiscard]] std::size_t quantityOf(ResourceType type) const;

  /// Add @p amount units of @p type.
  void add(ResourceType type, std::size_t amount = 1);

  /**
   * @brief Remove @p amount units of @p type.
   * @returns true if the player carried at least @p amount units and they
   *          were removed; false otherwise (nothing removed).
   */
  bool consume(ResourceType type, std::size_t amount = 1);

 private:
  std::array<std::size_t, resourceTypeCount> counts_{};
};

}  // namespace zappy::world
