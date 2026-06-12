/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TeamRegistry — owns every team, hatches and lays eggs
*/

#pragma once

#include <cstddef>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include "App/World/Team/Egg.hpp"
#include "App/World/Team/Team.hpp"

namespace zappy::world {

class Map;

/**
 * @brief Single owner of every Team in the game.
 *
 * Built from the command-line list of team names; offers the two operations
 * the rest of the server needs:
 * - @ref hatch: pick a random egg of a known team, remove it from the team and
 *   from its tile, and return it. Used by the AI handshake and by death-driven
 *   slot recycling.
 * - @ref lay: register a new egg for a known team, on the tile its parent
 *   stands on. Used by the Fork action.
 *
 * Eggs receive a stable numeric identifier from a monotonic counter so the GUI
 * protocol can refer to them later.
 */
class TeamRegistry {
 public:
  /**
   * @brief Build the registry with one empty team per name.
   * @param[in] teamNames Team names from `-n`; must be non-empty and unique.
   * @throws UnknownTeamException -> n/a here; uniqueness is enforced by the CLI
   *         parser upstream. Duplicate names just collapse silently.
   */
  explicit TeamRegistry(const std::vector<std::string>& teamNames);

  /// Whether @p teamName is known to this registry.
  [[nodiscard]] bool contains(const std::string& teamName) const;

  /// Free-slot count of @p teamName.
  /// @throws UnknownTeamException when the team is unknown.
  [[nodiscard]] std::size_t freeSlots(const std::string& teamName) const;

  /// Read-only view of every team, in command-line declaration order.
  [[nodiscard]] const std::vector<std::string>& names() const;

  /**
   * @brief Seed each team with @p slotsPerTeam eggs placed at random positions
   *        on @p map.
   *
   * Every spawned egg is also recorded on its tile via @ref Tile::addEgg so
   * GUI queries see it immediately.
   *
   * @param[in,out] map           World the eggs are laid on.
   * @param[in]     slotsPerTeam  Initial slot count per team (`-c`); must be
   *                              strictly positive.
   * @param[in,out] rng           Random source for egg positions.
   */
  void seedInitialEggs(Map& map, int slotsPerTeam, std::mt19937& rng);

  /**
   * @brief Hatch a random egg of @p teamName: remove it from the team and from
   *        its tile, then return it.
   *
   * @param[in]     teamName Team to draw from.
   * @param[in,out] map      World the egg sits on; its tile is updated.
   * @param[in,out] rng      Random source used to pick the egg.
   * @returns The hatched egg (id, team, x, y).
   * @throws UnknownTeamException when @p teamName is unknown.
   * @throws NoFreeSlotException  when the team has no egg left.
   */
  Egg hatch(const std::string& teamName, Map& map, std::mt19937& rng);

  /**
   * @brief Lay a new egg of @p teamName on tile (@p x, @p y).
   *
   * @param[in]     teamName Team gaining a slot.
   * @param[in,out] map      World the egg is laid on.
   * @param[in]     x        Tile column.
   * @param[in]     y        Tile row.
   * @returns The egg created (with its freshly minted id).
   * @throws UnknownTeamException when @p teamName is unknown.
   */
  Egg lay(const std::string& teamName, Map& map, int x, int y);

  /**
   * @brief Destroy the egg identified by @p eggId wherever it sits, dropping
   *        the owning team's free slot.
   *
   * Used when an ejection wipes the eggs off a tile. The tile bookkeeping is
   * the caller's responsibility; this only updates the owning team.
   *
   * @param[in] eggId Identifier of the egg to remove.
   * @returns true if some team owned an egg with @p eggId, false otherwise.
   */
  bool removeEgg(int eggId);

 private:
  [[nodiscard]] Team& teamOrThrow(const std::string& teamName);
  [[nodiscard]] const Team& teamOrThrow(const std::string& teamName) const;

  std::unordered_map<std::string, Team> teams_;
  std::vector<std::string> names_;
  int nextEggId_{1};
};

}  // namespace zappy::world
