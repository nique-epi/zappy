/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the ejectFromTile world operation
*/

#include <gtest/gtest.h>
#include "App/World/Ejection.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "App/World/Team/TeamRegistry.hpp"

using zappy::world::Direction;
using zappy::world::ejectFromTile;
using zappy::world::EjectOutcome;
using zappy::world::Map;
using zappy::world::PlayerRegistry;
using zappy::world::TeamRegistry;

TEST(Ejection, PushesEveryOtherDroneAndKeepsTheEjectorInPlace) {
  /*
   * Given an ejector facing North sharing tile (2, 2) with two other drones
   * When ejectFromTile runs for the ejector
   * Then both others move to (2, 1), the ejector stays, and both are reported
   */
  PlayerRegistry players;
  Map map(5, 5);
  TeamRegistry teams({"red"});
  const int ejector = players.spawn("red", 2, 2, Direction::North, map).id();
  const int firstVictim =
      players.spawn("blue", 2, 2, Direction::South, map).id();
  const int secondVictim =
      players.spawn("blue", 2, 2, Direction::East, map).id();

  const EjectOutcome outcome = ejectFromTile(ejector, players, map, teams);

  EXPECT_EQ(outcome.ejected.size(), 2U);
  EXPECT_EQ(players.find(ejector)->x(), 2);
  EXPECT_EQ(players.find(ejector)->y(), 2);
  EXPECT_EQ(players.find(firstVictim)->y(), 1);
  EXPECT_EQ(players.find(secondVictim)->y(), 1);
  EXPECT_EQ(map.tileAt(2, 2).players().size(), 1U);
  EXPECT_EQ(map.tileAt(2, 1).players().size(), 2U);
}

TEST(Ejection, ReportsEachVictimEjectCodeRelativeToItsFacing) {
  /*
   * Given an ejector facing North and a victim facing East on the same tile
   * When ejectFromTile runs
   * Then the victim is reported with eject code 7 (pushed from its right)
   */
  PlayerRegistry players;
  Map map(5, 5);
  TeamRegistry teams({"red"});
  const int ejector = players.spawn("red", 2, 2, Direction::North, map).id();
  const int victim = players.spawn("blue", 2, 2, Direction::East, map).id();

  const EjectOutcome outcome = ejectFromTile(ejector, players, map, teams);

  ASSERT_EQ(outcome.ejected.size(), 1U);
  EXPECT_EQ(outcome.ejected.front().playerId, victim);
  EXPECT_EQ(outcome.ejected.front().code, 7);
}

TEST(Ejection, AloneEjectorPushesNobody) {
  /*
   * Given an ejector standing alone on its tile
   * When ejectFromTile runs
   * Then no drone is reported as ejected
   */
  PlayerRegistry players;
  Map map(5, 5);
  TeamRegistry teams({"red"});
  const int ejector = players.spawn("red", 1, 1, Direction::West, map).id();

  const EjectOutcome outcome = ejectFromTile(ejector, players, map, teams);

  EXPECT_TRUE(outcome.ejected.empty());
}

TEST(Ejection, WrapsPushedDronesAroundTheToroidalEdge) {
  /*
   * Given an ejector on the western edge (0, 0) facing West with a victim
   * When ejectFromTile runs
   * Then the victim wraps to the opposite edge column
   */
  PlayerRegistry players;
  Map map(5, 5);
  TeamRegistry teams({"red"});
  const int ejector = players.spawn("red", 0, 0, Direction::West, map).id();
  const int victim = players.spawn("blue", 0, 0, Direction::North, map).id();

  const EjectOutcome outcome = ejectFromTile(ejector, players, map, teams);

  ASSERT_EQ(outcome.ejected.size(), 1U);
  EXPECT_EQ(players.find(victim)->x(), 4);
  EXPECT_EQ(players.find(victim)->y(), 0);
}

TEST(Ejection, DestroysEveryEggOnTheTileAndDropsTheirSlots) {
  /*
   * Given two red eggs laid on the ejector tile and one drone to push
   * When ejectFromTile runs
   * Then both eggs are destroyed, the team loses those slots and the tile is
   *      cleared of eggs
   */
  PlayerRegistry players;
  Map map(5, 5);
  TeamRegistry teams({"red"});
  teams.lay("red", map, 2, 2);
  teams.lay("red", map, 2, 2);
  const int ejector = players.spawn("red", 2, 2, Direction::North, map).id();
  players.spawn("blue", 2, 2, Direction::North, map);

  const EjectOutcome outcome = ejectFromTile(ejector, players, map, teams);

  EXPECT_EQ(outcome.eggsDestroyed, 2U);
  EXPECT_EQ(teams.freeSlots("red"), 0U);
  EXPECT_TRUE(map.tileAt(2, 2).eggs().empty());
}

TEST(Ejection, MissingEjectorIsANoOp) {
  /*
   * Given an id that matches no drone
   * When ejectFromTile runs
   * Then nothing is ejected and no egg is destroyed
   */
  PlayerRegistry players;
  Map map(5, 5);
  TeamRegistry teams({"red"});

  const EjectOutcome outcome = ejectFromTile(404, players, map, teams);

  EXPECT_TRUE(outcome.ejected.empty());
  EXPECT_EQ(outcome.eggsDestroyed, 0U);
}
