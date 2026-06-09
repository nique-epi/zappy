/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Player — a single drone in the world (position, direction, level, inventory)
*/

#pragma once

#include <string>
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Inventory.hpp"

namespace zappy::world {

/// Initial food units every player starts with (= 10).
constexpr int initialFoodUnits = 10;

/// Top elevation level reachable by a player.
constexpr int maxPlayerLevel = 8;

/**
 * @brief A drone in the world: identity, team, location, orientation, level,
 *        and personal inventory.
 *
 * Players occupy the whole tile they stand on; their orientation is not
 * observable by other drones. Level starts at 1, can climb to @ref
 * maxPlayerLevel through elevation rituals. The inventory pre-charges @ref
 * initialFoodUnits food units — the food reserve that the hunger loop will tick
 * down.
 */
class Player {
 public:
  /**
   * @brief Build a level-1 drone on (@p x, @p y) facing @p direction with the
   *        initial food reserve.
   */
  Player(int id, std::string teamName, int x, int y, Direction direction);

  [[nodiscard]] int id() const;
  [[nodiscard]] const std::string& teamName() const;
  [[nodiscard]] int x() const;
  [[nodiscard]] int y() const;
  [[nodiscard]] Direction direction() const;
  [[nodiscard]] int level() const;

  [[nodiscard]] const Inventory& inventory() const;
  [[nodiscard]] Inventory& inventory();

  void setPosition(int x, int y);
  void setDirection(Direction direction);
  void setLevel(int level);

 private:
  int id_;
  std::string teamName_;
  int x_;
  int y_;
  Direction direction_;
  int level_{1};
  Inventory inventory_;
};

}  // namespace zappy::world
