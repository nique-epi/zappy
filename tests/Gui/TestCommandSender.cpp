/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for CommandSender
*/

#include <gtest/gtest.h>
#include "FakeNetwork.hpp"
#include "Network/CommandSender.hpp"

using zappy::gui::test::FakeNetwork;

TEST(CommandSender, RequestMapSizeSendsMsz) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.requestMapSize();

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "msz");
}

TEST(CommandSender, RequestMapContentSendsMct) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.requestMapContent();

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "mct");
}

TEST(CommandSender, RequestTileContentSendsBctWithCoordinates) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.requestTileContent(3, 7);

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "bct 3 7");
}

TEST(CommandSender, RequestTeamNamesSendsTna) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.requestTeamNames();

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "tna");
}

TEST(CommandSender, RequestPlayerPositionSendsPpoWithId) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.requestPlayerPosition(42);

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "ppo #42");
}

TEST(CommandSender, RequestPlayerLevelSendsPlvWithId) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.requestPlayerLevel(42);

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "plv #42");
}

TEST(CommandSender, RequestPlayerInventorySendsPinWithId) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.requestPlayerInventory(42);

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "pin #42");
}

TEST(CommandSender, RequestTimeUnitSendsSgt) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.requestTimeUnit();

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "sgt");
}

TEST(CommandSender, SetTimeUnitSendsSstWithValue) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.setTimeUnit(50);

  ASSERT_EQ(network.sent().size(), 1U);
  EXPECT_EQ(network.sent()[0], "sst 50");
}

TEST(CommandSender, MultipleCommandsAreSentInOrder) {
  FakeNetwork network;
  zappy::gui::CommandSender sender(network);

  sender.requestMapSize();
  sender.requestMapContent();
  sender.requestTeamNames();

  const auto& sent = network.sent();
  ASSERT_EQ(sent.size(), 3U);
  EXPECT_EQ(sent[0], "msz");
  EXPECT_EQ(sent[1], "mct");
  EXPECT_EQ(sent[2], "tna");
}
