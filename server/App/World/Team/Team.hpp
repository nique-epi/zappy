/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Team — a named group owning a pool of eggs (free slots)
*/

#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "App/World/Team/Egg.hpp"

namespace zappy::world {

/**
 * @brief A team of drones identified by its name and tracking its pool of
 *        free slots, each represented by an Egg.
 *
 * Eggs are pushed onto the team during initial seeding (and later through the
 * Fork action) and popped from it on every successful AI connection (egg
 * hatching) or eviction. The free-slot count exposed to the AI at handshake
 * and via `Connect_nbr` is exactly @ref freeSlots.
 */
class Team {
 public:
  explicit Team(std::string name);

  /// Team identifier as declared on the command line.
  [[nodiscard]] const std::string& name() const;

  /// Number of free slots the team has right now (= number of pending eggs).
  [[nodiscard]] std::size_t freeSlots() const;

  /// Whether the team currently has no free slot.
  [[nodiscard]] bool empty() const;

  /// Read-only view of the eggs the team owns.
  [[nodiscard]] const std::vector<Egg>& eggs() const;

  /**
   * @brief Append @p egg to the team's pool (initial seeding or Fork).
   */
  void addEgg(Egg egg);

  /**
   * @brief Remove the egg at @p index and return it (hatching / eviction).
   * @throws std::out_of_range when @p index is past the end. Callers guarantee
   *         the index comes from a random draw bounded by @ref freeSlots, so
   *         this is a programming error.
   */
  Egg removeEggAt(std::size_t index);

 private:
  std::string name_;
  std::vector<Egg> eggs_;
};

}  // namespace zappy::world
