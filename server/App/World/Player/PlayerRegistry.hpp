/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerRegistry — single owner of every living drone in the world
*/

#pragma once

#include <cstddef>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"

namespace zappy::world {

class Map;

/**
 * @brief Single owner of every Player currently in the world.
 *
 * Mints a stable numeric identifier per drone, keeps drones addressable by id
 * (the GUI protocol uses `#n` to refer to them), and pushes their presence to
 * the underlying tile so per-tile queries see them immediately. Drones are
 * removed from the registry — and from their tile — when they die or
 * disconnect.
 */
class PlayerRegistry {
 public:
  /// Number of drones currently alive.
  [[nodiscard]] std::size_t size() const;

  /// Whether a drone with @p playerId is currently registered.
  [[nodiscard]] bool contains(int playerId) const;

  /**
   * @brief Read-only access to the drone with @p playerId.
   * @returns Pointer to the drone, or nullptr when no drone matches.
   */
  [[nodiscard]] const Player* find(int playerId) const;

  /// Mutable access; same null contract as @ref find.
  [[nodiscard]] Player* find(int playerId);

  /**
   * @brief Hatch a new drone of @p teamName on (@p x, @p y) facing @p
   * direction.
   *
   * Adds the player to the tile via @ref Tile::addPlayer so callers do not need
   * to touch the map themselves.
   *
   * @returns A reference to the freshly registered drone.
   */
  Player& spawn(const std::string& teamName, int x, int y, Direction direction,
                Map& map);

  /**
   * @brief Pick a uniformly random direction.
   */
  static Direction randomDirection(std::mt19937& rng);

  /**
   * @brief Remove the drone with @p playerId from the registry and from its
   *        current tile.
   * @returns true if the drone existed and was removed; false otherwise.
   */
  bool remove(int playerId, Map& map);

  /**
   * @brief Identifiers of every currently registered drone, in insertion
   *        order.
   */
  [[nodiscard]] std::vector<int> ids() const;

 private:
  std::unordered_map<int, std::unique_ptr<Player>> players_;
  std::vector<int> insertionOrder_;
  int nextPlayerId_{1};
};

}  // namespace zappy::world
