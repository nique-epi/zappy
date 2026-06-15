/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the findWinningTeam victory-detection world helper
*/

#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <string>
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "App/World/Victory.hpp"

using zappy::world::Direction;
using zappy::world::dronesRequiredToWin;
using zappy::world::findWinningTeam;
using zappy::world::Map;
using zappy::world::maxPlayerLevel;
using zappy::world::PlayerRegistry;

namespace {

void spawnDrones(PlayerRegistry& players, Map& map, const std::string& team,
                 std::size_t count, int level) {
  for (std::size_t index = 0; index < count; index++) {
    zappy::world::Player& player =
        players.spawn(team, 0, 0, Direction::North, map);
    player.setLevel(level);
  }
}

}  // namespace

TEST(Victory, NoWinnerOnAnEmptyWorld) {
  /*
   * Given a registry with no drone
   * When the winning team is queried
   * Then no team qualifies
   */
  PlayerRegistry players;
  Map map(5, 5);

  EXPECT_FALSE(findWinningTeam(players).has_value());
}

TEST(Victory, NoWinnerBelowTheThreshold) {
  /*
   * Given one drone short of the threshold maxed out in a team
   * When the winning team is queried
   * Then no team qualifies
   */
  PlayerRegistry players;
  Map map(5, 5);
  spawnDrones(players, map, "red", dronesRequiredToWin - 1, maxPlayerLevel);

  EXPECT_FALSE(findWinningTeam(players).has_value());
}

TEST(Victory, WinsWhenEnoughDronesReachTheMaxLevel) {
  /*
   * Given exactly the threshold of max-level drones in one team
   * When the winning team is queried
   * Then that team is reported as the winner
   */
  PlayerRegistry players;
  Map map(5, 5);
  spawnDrones(players, map, "red", dronesRequiredToWin, maxPlayerLevel);

  const std::optional<std::string> winner = findWinningTeam(players);
  ASSERT_TRUE(winner.has_value());
  EXPECT_EQ(*winner, "red");
}

TEST(Victory, IgnoresDronesBelowTheMaxLevel) {
  /*
   * Given a team with the threshold count but only one drone short of max level
   * When the winning team is queried
   * Then no team qualifies because under-level drones do not count
   */
  PlayerRegistry players;
  Map map(5, 5);
  spawnDrones(players, map, "red", dronesRequiredToWin - 1, maxPlayerLevel);
  spawnDrones(players, map, "red", 1, maxPlayerLevel - 1);

  EXPECT_FALSE(findWinningTeam(players).has_value());
}

TEST(Victory, DoesNotWinWhenMaxedDronesAreSplitAcrossTeams) {
  /*
   * Given the threshold count of max-level drones split between two teams
   * When the winning team is queried
   * Then no single team reaches the threshold
   */
  PlayerRegistry players;
  Map map(5, 5);
  spawnDrones(players, map, "red", dronesRequiredToWin - 1, maxPlayerLevel);
  spawnDrones(players, map, "blue", dronesRequiredToWin - 1, maxPlayerLevel);

  EXPECT_FALSE(findWinningTeam(players).has_value());
}

TEST(Victory, ReportsTheTeamThatReachesTheThreshold) {
  /*
   * Given one team short of the threshold and another that meets it
   * When the winning team is queried
   * Then the team that met the threshold is reported
   */
  PlayerRegistry players;
  Map map(5, 5);
  spawnDrones(players, map, "red", dronesRequiredToWin - 1, maxPlayerLevel);
  spawnDrones(players, map, "blue", dronesRequiredToWin, maxPlayerLevel);

  const std::optional<std::string> winner = findWinningTeam(players);
  ASSERT_TRUE(winner.has_value());
  EXPECT_EQ(*winner, "blue");
}
