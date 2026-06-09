/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit and functional tests for MessageParser
*/

#include <gtest/gtest.h>
#include <string>
#include "Network/MessageParser.hpp"
#include "World/WorldState.hpp"

TEST(MessageParser, MszSetsWidthAndHeight) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 10 20");

  EXPECT_EQ(world.width, 10);
  EXPECT_EQ(world.height, 20);
}

TEST(MessageParser, MszAllocatesTileGrid) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 5 3");

  ASSERT_EQ(static_cast<int>(world.tiles.size()), 3);
  EXPECT_EQ(static_cast<int>(world.tiles[0].size()), 5);
}

TEST(MessageParser, MszInitializesTileCoordinates) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 3 2");

  EXPECT_EQ(world.tiles[0][0].x, 0);
  EXPECT_EQ(world.tiles[0][0].y, 0);
  EXPECT_EQ(world.tiles[1][2].x, 2);
  EXPECT_EQ(world.tiles[1][2].y, 1);
}

TEST(MessageParser, BctUpdatesTileResources) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 2 2");
  parser.parseLine("bct 1 0 1 2 3 4 5 6 7");

  const auto& resources = world.tiles[0][1].resources;
  EXPECT_EQ(resources[0], 1);
  EXPECT_EQ(resources[6], 7);
}

TEST(MessageParser, BctOutOfBoundsIsIgnored) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 2 2");

  EXPECT_NO_THROW(parser.parseLine("bct 99 99 0 0 0 0 0 0 0"));
}

TEST(MessageParser, PnwAddsPlayer) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("pnw #1 3 4 2 5 team1");

  ASSERT_EQ(world.players.size(), 1U);
  const auto& player = world.players[0];
  EXPECT_EQ(player.id, 1);
  EXPECT_EQ(player.x, 3);
  EXPECT_EQ(player.y, 4);
  EXPECT_EQ(player.orientation, zappy::gui::Orientation::East);
  EXPECT_EQ(player.level, 5);
  EXPECT_EQ(player.teamName, "team1");
}

TEST(MessageParser, PnwWithPlainIdAddsPlayer) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("pnw 2 0 0 1 1 alpha");

  ASSERT_EQ(world.players.size(), 1U);
  EXPECT_EQ(world.players[0].id, 2);
}

TEST(MessageParser, PpoUpdatesPlayerPositionAndOrientation) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("pnw #1 0 0 1 1 team1");
  parser.parseLine("ppo #1 5 6 3");

  const auto& player = world.players[0];
  EXPECT_EQ(player.x, 5);
  EXPECT_EQ(player.y, 6);
  EXPECT_EQ(player.orientation, zappy::gui::Orientation::South);
}

TEST(MessageParser, PpoOnUnknownPlayerIsIgnored) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  EXPECT_NO_THROW(parser.parseLine("ppo #99 1 2 1"));
}

TEST(MessageParser, PlvUpdatesPlayerLevel) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("pnw #1 0 0 1 1 team1");
  parser.parseLine("plv #1 8");

  EXPECT_EQ(world.players[0].level, 8);
}

TEST(MessageParser, PinUpdatesPlayerPositionAndInventory) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 3 3");
  parser.parseLine("pnw #1 0 0 1 1 team1");
  parser.parseLine("pin #1 1 2 10 20 30 40 50 60 70");

  const auto& player = world.players[0];
  EXPECT_EQ(player.x, 1);
  EXPECT_EQ(player.y, 2);
  EXPECT_EQ(player.inventory[0], 10);
  EXPECT_EQ(player.inventory[6], 70);
}

TEST(MessageParser, PdiRemovesPlayer) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("pnw #1 0 0 1 1 team1");
  parser.parseLine("pdi #1");

  EXPECT_TRUE(world.players.empty());
}

TEST(MessageParser, PdiOnlyRemovesTargetPlayer) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("pnw #1 0 0 1 1 team1");
  parser.parseLine("pnw #2 0 0 1 1 team1");
  parser.parseLine("pdi #1");

  ASSERT_EQ(world.players.size(), 1U);
  EXPECT_EQ(world.players[0].id, 2);
}

TEST(MessageParser, PdrTransfersResourceFromPlayerToTile) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 3 3");
  parser.parseLine("pnw #1 1 1 1 1 team1");
  parser.parseLine("pin #1 1 1 5 0 0 0 0 0 0");
  parser.parseLine("pdr #1 0");

  EXPECT_EQ(world.tiles[1][1].resources[0], 1);
  EXPECT_EQ(world.players[0].inventory[0], 4);
}

TEST(MessageParser, PgtTransfersResourceFromTileToPlayer) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 3 3");
  parser.parseLine("bct 1 1 0 0 10 0 0 0 0");
  parser.parseLine("pnw #1 1 1 1 1 team1");
  parser.parseLine("pgt #1 2");

  EXPECT_EQ(world.tiles[1][1].resources[2], 9);
  EXPECT_EQ(world.players[0].inventory[2], 1);
}

TEST(MessageParser, EnwAddsEgg) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #10 #1 3 5");

  ASSERT_EQ(world.eggs.size(), 1U);
  const auto& egg = world.eggs[0];
  EXPECT_EQ(egg.id, 10);
  EXPECT_EQ(egg.playerId, 1);
  EXPECT_EQ(egg.x, 3);
  EXPECT_EQ(egg.y, 5);
}

TEST(MessageParser, EboRemovesEgg) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #10 #1 0 0");
  parser.parseLine("ebo #10");

  EXPECT_TRUE(world.eggs.empty());
}

TEST(MessageParser, EdiRemovesEgg) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #10 #1 0 0");
  parser.parseLine("edi #10");

  EXPECT_TRUE(world.eggs.empty());
}

TEST(MessageParser, EboOnlyRemovesTargetEgg) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #10 #1 0 0");
  parser.parseLine("enw #11 #2 0 0");
  parser.parseLine("ebo #10");

  ASSERT_EQ(world.eggs.size(), 1U);
  EXPECT_EQ(world.eggs[0].id, 11);
}

TEST(MessageParser, TnaAddsTeamName) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("tna teamA");

  ASSERT_EQ(world.teams.size(), 1U);
  EXPECT_EQ(world.teams[0], "teamA");
}

TEST(MessageParser, TnaAccumulatesMultipleTeams) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("tna alpha");
  parser.parseLine("tna beta");
  parser.parseLine("tna gamma");

  ASSERT_EQ(world.teams.size(), 3U);
  EXPECT_EQ(world.teams[0], "alpha");
  EXPECT_EQ(world.teams[2], "gamma");
}

TEST(MessageParser, SgtSetsTimeUnit) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("sgt 100");

  EXPECT_EQ(world.timeUnit, 100);
}

TEST(MessageParser, SstSetsTimeUnit) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("sst 200");

  EXPECT_EQ(world.timeUnit, 200);
}

TEST(MessageParser, SegSetsGameOverAndWinnerTeam) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("seg teamX");

  EXPECT_TRUE(world.gameOver);
  EXPECT_EQ(world.winnerTeam, "teamX");
}

TEST(MessageParser, SucDoesNotCrash) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  EXPECT_NO_THROW(parser.parseLine("suc"));
}

TEST(MessageParser, SbpDoesNotCrash) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  EXPECT_NO_THROW(parser.parseLine("sbp"));
}

TEST(MessageParser, NoOpCommandsDoNotCrash) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  EXPECT_NO_THROW(parser.parseLine("pex #1"));
  EXPECT_NO_THROW(parser.parseLine("pbc #1 hello"));
  EXPECT_NO_THROW(parser.parseLine("pic 0 0 1 #1 #2"));
  EXPECT_NO_THROW(parser.parseLine("pie 0 0 1"));
  EXPECT_NO_THROW(parser.parseLine("pfk #1"));
  EXPECT_NO_THROW(parser.parseLine("smg hello world"));
}

TEST(MessageParser, UnknownCommandIsIgnored) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  EXPECT_NO_THROW(parser.parseLine("xyz 1 2 3"));

  EXPECT_TRUE(world.players.empty());
  EXPECT_TRUE(world.eggs.empty());
  EXPECT_TRUE(world.teams.empty());
}

TEST(MessageParser, EmptyLineIsIgnored) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  EXPECT_NO_THROW(parser.parseLine(""));
}

TEST(MessageParser, FullMapInitSequence) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 2 2");
  parser.parseLine("bct 0 0 1 0 0 0 0 0 0");
  parser.parseLine("bct 1 0 0 2 0 0 0 0 0");
  parser.parseLine("bct 0 1 0 0 3 0 0 0 0");
  parser.parseLine("bct 1 1 0 0 0 4 0 0 0");

  EXPECT_EQ(world.tiles[0][0].resources[0], 1);
  EXPECT_EQ(world.tiles[0][1].resources[1], 2);
  EXPECT_EQ(world.tiles[1][0].resources[2], 3);
  EXPECT_EQ(world.tiles[1][1].resources[3], 4);
}

TEST(MessageParser, PlayerLifecycleSequence) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("pnw #1 0 0 1 1 team1");
  ASSERT_EQ(world.players.size(), 1U);

  parser.parseLine("ppo #1 3 4 2");
  EXPECT_EQ(world.players[0].x, 3);
  EXPECT_EQ(world.players[0].y, 4);

  parser.parseLine("plv #1 7");
  EXPECT_EQ(world.players[0].level, 7);

  parser.parseLine("pdi #1");
  EXPECT_TRUE(world.players.empty());
}

TEST(MessageParser, EggLifecycleHatch) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("enw #5 #1 2 3");
  ASSERT_EQ(world.eggs.size(), 1U);

  parser.parseLine("ebo #5");
  EXPECT_TRUE(world.eggs.empty());
}

TEST(MessageParser, ResourceTransferRoundTrip) {
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);

  parser.parseLine("msz 3 3");
  parser.parseLine("bct 1 1 5 0 0 0 0 0 0");
  parser.parseLine("pnw #1 1 1 1 1 team1");
  parser.parseLine("pgt #1 0");

  EXPECT_EQ(world.tiles[1][1].resources[0], 4);
  EXPECT_EQ(world.players[0].inventory[0], 1);

  parser.parseLine("pdr #1 0");

  EXPECT_EQ(world.tiles[1][1].resources[0], 5);
  EXPECT_EQ(world.players[0].inventory[0], 0);
}
