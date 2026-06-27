/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for computeTeamProgress victory-progress aggregation
*/

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "World/WorldQuery.hpp"
#include "World/WorldState.hpp"

using zappy::gui::computeTeamProgress;
using zappy::gui::Player;
using zappy::gui::TeamProgress;
using zappy::gui::WorldState;

namespace {

Player makePlayer(int id, const std::string& team, int level, bool alive) {
  return Player{.id = id, .level = level, .teamName = team, .alive = alive};
}

}  // namespace

// Given a world with no teams registered
// When the team progress is computed
// Then the result is empty
TEST(ComputeTeamProgress, NoTeamsYieldsEmpty) {
  WorldState world;

  EXPECT_TRUE(computeTeamProgress(world).empty());
}

// Given two teams whose players have mixed levels and liveness
// When the team progress is computed
// Then each team reports its living players and its level-8 players only
TEST(ComputeTeamProgress, CountsAliveAndMaxLevelPerTeam) {
  WorldState world;
  world.teams = {"red", "blue"};
  world.players = {
      makePlayer(1, "red", zappy::gui::MAX_PLAYER_LEVEL, true),
      makePlayer(2, "red", zappy::gui::MAX_PLAYER_LEVEL, true),
      makePlayer(3, "red", 3, true),
      makePlayer(4, "red", zappy::gui::MAX_PLAYER_LEVEL, false),
      makePlayer(5, "blue", 1, true),
  };

  const std::vector<TeamProgress> progress = computeTeamProgress(world);

  ASSERT_EQ(progress.size(), 2U);
  EXPECT_EQ(progress[0].name, "red");
  EXPECT_EQ(progress[0].aliveCount, 3);
  EXPECT_EQ(progress[0].maxLevelCount, 2);
  EXPECT_EQ(progress[1].name, "blue");
  EXPECT_EQ(progress[1].aliveCount, 1);
  EXPECT_EQ(progress[1].maxLevelCount, 0);
}

// Given teams registered in a specific arrival order, one without players
// When the team progress is computed
// Then teams keep their arrival order and empty teams report zero counts
TEST(ComputeTeamProgress, PreservesTeamOrderAndZeroesEmptyTeams) {
  WorldState world;
  world.teams = {"gamma", "alpha"};
  world.players = {makePlayer(1, "alpha", zappy::gui::MAX_PLAYER_LEVEL, true)};

  const std::vector<TeamProgress> progress = computeTeamProgress(world);

  ASSERT_EQ(progress.size(), 2U);
  EXPECT_EQ(progress[0].name, "gamma");
  EXPECT_EQ(progress[0].aliveCount, 0);
  EXPECT_EQ(progress[0].maxLevelCount, 0);
  EXPECT_EQ(progress[1].name, "alpha");
  EXPECT_EQ(progress[1].aliveCount, 1);
  EXPECT_EQ(progress[1].maxLevelCount, 1);
}
