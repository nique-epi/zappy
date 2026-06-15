/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the resolveIncantation world operation
*/

#include <gtest/gtest.h>
#include "App/World/Incantation/Incantation.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "App/World/Resources/ResourceType.hpp"

using zappy::world::Direction;
using zappy::world::IncantationOutcome;
using zappy::world::Map;
using zappy::world::PlayerRegistry;
using zappy::world::resolveIncantation;
using zappy::world::ResourceType;

TEST(Incantation, FirstLevelRitualRaisesTheSoloInitiatorAndDrainsTheLinemate) {
  /*
   * Given a lone level-1 drone on a tile holding one linemate
   * When the incantation resolves
   * Then it succeeds, the drone reaches level 2 and the linemate is consumed
   */
  PlayerRegistry players;
  Map map(5, 5);
  const int drone = players.spawn("red", 2, 2, Direction::North, map).id();
  map.tileAt(2, 2).drop(ResourceType::Linemate, 1);

  const IncantationOutcome outcome = resolveIncantation(drone, players, map);

  EXPECT_TRUE(outcome.succeeded);
  EXPECT_EQ(outcome.newLevel, 2);
  EXPECT_EQ(outcome.participants.size(), 1U);
  EXPECT_EQ(players.find(drone)->level(), 2);
  EXPECT_EQ(map.tileAt(2, 2).quantityOf(ResourceType::Linemate), 0U);
}

TEST(Incantation, RaisesEverySameLevelDroneSharingTheTile) {
  /*
   * Given two level-2 drones together on a tile with the 2->3 minerals
   * When the incantation resolves
   * Then both reach level 3 and every required mineral is removed
   */
  PlayerRegistry players;
  Map map(5, 5);
  const int first = players.spawn("red", 1, 1, Direction::North, map).id();
  const int second = players.spawn("blue", 1, 1, Direction::South, map).id();
  players.find(first)->setLevel(2);
  players.find(second)->setLevel(2);
  map.tileAt(1, 1).drop(ResourceType::Linemate, 1);
  map.tileAt(1, 1).drop(ResourceType::Deraumere, 1);
  map.tileAt(1, 1).drop(ResourceType::Sibur, 1);

  const IncantationOutcome outcome = resolveIncantation(first, players, map);

  EXPECT_TRUE(outcome.succeeded);
  EXPECT_EQ(outcome.newLevel, 3);
  EXPECT_EQ(outcome.participants.size(), 2U);
  EXPECT_EQ(players.find(first)->level(), 3);
  EXPECT_EQ(players.find(second)->level(), 3);
  EXPECT_EQ(map.tileAt(1, 1).quantityOf(ResourceType::Linemate), 0U);
  EXPECT_EQ(map.tileAt(1, 1).quantityOf(ResourceType::Deraumere), 0U);
  EXPECT_EQ(map.tileAt(1, 1).quantityOf(ResourceType::Sibur), 0U);
}

TEST(Incantation, IgnoresDronesOfADifferentLevelOnTheTile) {
  /*
   * Given a level-2 initiator alone at its level, joined by a level-1 drone
   * When the incantation resolves
   * Then the prerequisites are unmet and the ritual fails without mutating
   */
  PlayerRegistry players;
  Map map(5, 5);
  const int initiator = players.spawn("red", 3, 3, Direction::North, map).id();
  const int other = players.spawn("blue", 3, 3, Direction::North, map).id();
  players.find(initiator)->setLevel(2);
  map.tileAt(3, 3).drop(ResourceType::Linemate, 1);
  map.tileAt(3, 3).drop(ResourceType::Deraumere, 1);
  map.tileAt(3, 3).drop(ResourceType::Sibur, 1);

  const IncantationOutcome outcome =
      resolveIncantation(initiator, players, map);

  EXPECT_FALSE(outcome.succeeded);
  EXPECT_TRUE(outcome.participants.empty());
  EXPECT_EQ(players.find(initiator)->level(), 2);
  EXPECT_EQ(players.find(other)->level(), 1);
  EXPECT_EQ(map.tileAt(3, 3).quantityOf(ResourceType::Linemate), 1U);
}

TEST(Incantation, FailsWhenMineralsAreMissing) {
  /*
   * Given a lone level-1 drone on an empty tile
   * When the incantation resolves
   * Then it fails and the drone stays at level 1
   */
  PlayerRegistry players;
  Map map(5, 5);
  const int drone = players.spawn("red", 0, 0, Direction::East, map).id();

  const IncantationOutcome outcome = resolveIncantation(drone, players, map);

  EXPECT_FALSE(outcome.succeeded);
  EXPECT_EQ(players.find(drone)->level(), 1);
}

TEST(Incantation, FailsWhenInitiatorIsAlreadyAtTheMaximumLevel) {
  /*
   * Given a level-8 drone on a tile full of minerals
   * When the incantation resolves
   * Then it fails because no elevation step exists beyond level 8
   */
  PlayerRegistry players;
  Map map(5, 5);
  const int drone = players.spawn("red", 4, 4, Direction::West, map).id();
  players.find(drone)->setLevel(8);
  map.tileAt(4, 4).drop(ResourceType::Linemate, 5);

  const IncantationOutcome outcome = resolveIncantation(drone, players, map);

  EXPECT_FALSE(outcome.succeeded);
  EXPECT_EQ(players.find(drone)->level(), 8);
}

TEST(Incantation, MissingInitiatorIsANoOp) {
  /*
   * Given an id that matches no drone
   * When the incantation resolves
   * Then it fails with no participants
   */
  PlayerRegistry players;
  Map map(5, 5);

  const IncantationOutcome outcome = resolveIncantation(404, players, map);

  EXPECT_FALSE(outcome.succeeded);
  EXPECT_TRUE(outcome.participants.empty());
}
