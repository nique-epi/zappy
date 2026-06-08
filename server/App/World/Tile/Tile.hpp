/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Tile — content of a single cell of the world
*/

#pragma once

#include <array>
#include <cstddef>
#include <vector>
#include "App/World/Resources/ResourceType.hpp"

namespace zappy::world {

/**
 * @brief Everything sitting on one cell of the world: resource stocks, the
 *        players standing on it and the eggs laid on it.
 *
 * Resources are stored as per-kind counts (cf. ResourceType): two units of the
 * same kind are indistinguishable. Players and eggs are referenced by their
 * numeric identifier, so the tile never owns those entities — it only records
 * who currently stands or rests on it.
 */
class Tile {
 public:
  Tile() = default;

  /**
   * @brief Number of units of @p type currently on the tile.
   * @param[in] type The resource kind to look up.
   * @returns The stock of @p type, zero if none is present.
   */
  [[nodiscard]] std::size_t quantityOf(ResourceType type) const;

  /**
   * @brief Add @p amount units of @p type onto the tile (resource spawn or the
   *        AI `Set` command).
   * @param[in] type   The resource kind to deposit.
   * @param[in] amount Number of units to add.
   */
  void drop(ResourceType type, std::size_t amount = 1);

  /**
   * @brief Remove @p amount units of @p type from the tile (the AI `Take`
   *        command).
   * @param[in] type   The resource kind to pick up.
   * @param[in] amount Number of units to remove.
   * @returns true if the tile held at least @p amount units and they were
   *          removed; false if the request exceeds the stock, in which case
   *          nothing is removed.
   */
  bool collect(ResourceType type, std::size_t amount = 1);

  /**
   * @brief Identifiers of the players currently standing on the tile.
   */
  [[nodiscard]] const std::vector<int>& players() const;

  /**
   * @brief Record that player @p playerId now stands on the tile.
   */
  void addPlayer(int playerId);

  /**
   * @brief Remove player @p playerId from the tile.
   * @returns true if the player was present and removed, false otherwise.
   */
  bool removePlayer(int playerId);

  /**
   * @brief Identifiers of the eggs currently laid on the tile.
   */
  [[nodiscard]] const std::vector<int>& eggs() const;

  /**
   * @brief Record that egg @p eggId now rests on the tile.
   */
  void addEgg(int eggId);

  /**
   * @brief Remove egg @p eggId from the tile.
   * @returns true if the egg was present and removed, false otherwise.
   */
  bool removeEgg(int eggId);

 private:
  std::array<std::size_t, resourceTypeCount> resources_{};
  std::vector<int> players_;
  std::vector<int> eggs_;
};

}  // namespace zappy::world
