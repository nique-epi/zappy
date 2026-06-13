/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit and functional tests for WorldInitializer
*/

#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include "Exceptions/GuiException.hpp"
#include "Exceptions/WorldInitException.hpp"
#include "FakeNetwork.hpp"
#include "Network/Parsing/MessageParser.hpp"
#include "World/WorldInitializer.hpp"
#include "World/WorldState.hpp"

using zappy::gui::test::FakeNetwork;

namespace {

void injectMapSize(FakeNetwork& network, int width, int height) {
  network.inject("msz " + std::to_string(width) + " " + std::to_string(height));
}

void injectTileContent(FakeNetwork& network, int posX, int posY) {
  network.inject("bct " + std::to_string(posX) + " " + std::to_string(posY) +
                 " 0 0 0 0 0 0 0");
}

}  // namespace

TEST(WorldInitializer, IsNotDoneInitially) {
  FakeNetwork network;
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world);

  EXPECT_FALSE(initializer.isDone());
}

TEST(WorldInitializer, ConstructionSendsMapSizeRequest) {
  FakeNetwork network;
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world);

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "msz");
}

TEST(WorldInitializer, MapSizeResponseRequestsMapContent) {
  FakeNetwork network;
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world);

  injectMapSize(network, 2, 2);

  ASSERT_EQ(network.sent().size(), 2U);
  EXPECT_EQ(network.sent()[1], "mct");
  EXPECT_EQ(world.width, 2);
  EXPECT_EQ(world.height, 2);
  EXPECT_FALSE(initializer.isDone());
}

TEST(WorldInitializer, AllTileResponsesRequestTeamNames) {
  FakeNetwork network;
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world);

  injectMapSize(network, 2, 2);
  injectTileContent(network, 0, 0);
  injectTileContent(network, 1, 0);
  injectTileContent(network, 0, 1);
  EXPECT_EQ(network.sent().size(), 2U);

  injectTileContent(network, 1, 1);

  ASSERT_EQ(network.sent().size(), 3U);
  EXPECT_EQ(network.sent()[2], "tna");
}

TEST(WorldInitializer, ZeroSizeMapSkipsMapContent) {
  FakeNetwork network;
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world);

  injectMapSize(network, 0, 0);

  ASSERT_EQ(network.sent().size(), 2U);
  EXPECT_EQ(network.sent()[1], "tna");
}

TEST(WorldInitializer, CompletesAfterPollRoundWithoutNewTeamName) {
  FakeNetwork network;
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world);

  injectMapSize(network, 0, 0);
  network.inject("tna alpha");
  network.inject("tna beta");

  initializer.onPollRoundComplete();
  EXPECT_FALSE(initializer.isDone());

  initializer.onPollRoundComplete();
  EXPECT_TRUE(initializer.isDone());
}

TEST(WorldInitializer, FullSequencePopulatesWorldState) {
  FakeNetwork network;
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world);

  injectMapSize(network, 2, 1);
  network.inject("bct 0 0 1 0 0 0 0 0 0");
  network.inject("bct 1 0 0 2 0 0 0 0 0");
  network.inject("tna alpha");
  network.inject("tna beta");
  initializer.onPollRoundComplete();
  initializer.onPollRoundComplete();

  EXPECT_TRUE(initializer.isDone());
  EXPECT_EQ(world.width, 2);
  EXPECT_EQ(world.height, 1);
  EXPECT_EQ(world.tiles[0][0].resources[0], 1);
  EXPECT_EQ(world.tiles[0][1].resources[1], 2);
  ASSERT_EQ(world.teams.size(), 2U);
  EXPECT_EQ(world.teams[0], "alpha");
  EXPECT_EQ(world.teams[1], "beta");
}

TEST(WorldInitializer, CheckTimeoutIsNoOpWhenDone) {
  FakeNetwork network;
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world, 0);

  injectMapSize(network, 0, 0);
  network.inject("tna alpha");
  initializer.onPollRoundComplete();
  initializer.onPollRoundComplete();

  ASSERT_TRUE(initializer.isDone());
  EXPECT_NO_THROW(initializer.checkTimeout());
}

TEST(WorldInitializer, CheckTimeoutThrowsWorldInitTimeoutException) {
  FakeNetwork network;
  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world, 0);

  EXPECT_THROW(initializer.checkTimeout(),
               zappy::gui::WorldInitTimeoutException);
}

TEST(WorldInitializer, TimeoutExceptionIsAWorldInitException) {
  EXPECT_THROW(throw zappy::gui::WorldInitTimeoutException(),
               zappy::gui::WorldInitException);
}

TEST(WorldInitializer, WorldInitExceptionIsAGuiException) {
  EXPECT_THROW(throw zappy::gui::WorldInitTimeoutException(),
               zappy::gui::GuiException);
}

TEST(WorldInitializer, GuiExceptionIsAStdRuntimeError) {
  EXPECT_THROW(throw zappy::gui::WorldInitTimeoutException(),
               std::runtime_error);
}

TEST(WorldInitializer, TimeoutExceptionMessageIsPreserved) {
  const std::string msg =
      "server did not complete the world initialization sequence within "
      "the timeout";
  EXPECT_EQ(std::string(zappy::gui::WorldInitTimeoutException().what()), msg);
}
