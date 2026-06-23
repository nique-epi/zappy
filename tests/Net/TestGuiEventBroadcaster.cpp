/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Integration tests for GuiEventBroadcaster
*/

#include <gtest/gtest.h>
#include <string>
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Team/Egg.hpp"
#include "App/World/Tile/Tile.hpp"
#include "LoopbackClient.hpp"
#include "Net/ClientContext.hpp"
#include "Net/Gui/GuiEventBroadcaster.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Server/RPCServer.hpp"
#include "Rpc/Session/Session.hpp"

using zappy::rpc::IMessage;
using zappy::rpc::RPCServer;
using zappy::rpc::Session;
using zappy::rpc::test::LoopbackClient;
using zappy::server::ClientContext;
using zappy::server::ClientType;
using zappy::server::GuiEventBroadcaster;
using zappy::world::Direction;
using zappy::world::Egg;
using zappy::world::Map;
using zappy::world::Player;
using zappy::world::PlayerRegistry;
using zappy::world::ResourceType;

namespace {

constexpr int driveIterations = 4;
constexpr int driveTimeoutMs = 20;
constexpr int mapSide = 8;
constexpr const char* graphicTeam = "GRAPHIC";

struct BroadcasterHarness {
  Map map;
  PlayerRegistry players;
  RPCServer<ClientContext> server;
  GuiEventBroadcaster gui;

  BroadcasterHarness(int width, int height)
      : map(width, height), server(0), gui(server) {
    server.onHandshake(
        [](Session<ClientContext>& session, const std::string& teamName) {
          session.ctx().type =
              teamName == graphicTeam ? ClientType::Gui : ClientType::Ai;
          session.completeHandshake();
        });
    server.onUnknown([](Session<ClientContext>& session, IMessage&) {
      session.send("sentinel");
    });
    server.start();
  }

  void drive(int iterations = driveIterations) {
    for (int index = 0; index < iterations; index++) {
      server.runOnce(driveTimeoutMs);
    }
  }
};

void handshake(BroadcasterHarness& harness, LoopbackClient& client,
               const std::string& teamName) {
  harness.drive();
  client.sendLine(teamName);
  harness.drive();
}

}  // namespace

TEST(GuiEventBroadcaster, PlayerConnectedEmitsPnw) {
  /*
   * Given a GUI client and a drone spawned at (2, 3) facing North
   * When playerConnected is broadcast
   * Then the client receives "pnw #1 2 3 1 1 red"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);
  const Player& player =
      harness.players.spawn("red", 2, 3, Direction::North, harness.map);

  harness.gui.playerConnected(player);
  harness.drive();

  EXPECT_EQ(client.receive(), "pnw #1 2 3 1 1 red\n");
}

TEST(GuiEventBroadcaster, PlayerMovedEmitsPpo) {
  /*
   * Given a GUI client and a drone at (4, 1) facing East
   * When playerMoved is broadcast
   * Then the client receives "ppo #1 4 1 2"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);
  const Player& player =
      harness.players.spawn("red", 4, 1, Direction::East, harness.map);

  harness.gui.playerMoved(player);
  harness.drive();

  EXPECT_EQ(client.receive(), "ppo #1 4 1 2\n");
}

TEST(GuiEventBroadcaster, PlayerLevelEmitsPlv) {
  /*
   * Given a GUI client and a drone raised to level 2
   * When playerLevel is broadcast
   * Then the client receives "plv #1 2"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);
  Player& player =
      harness.players.spawn("red", 0, 0, Direction::North, harness.map);
  player.setLevel(2);

  harness.gui.playerLevel(player);
  harness.drive();

  EXPECT_EQ(client.receive(), "plv #1 2\n");
}

TEST(GuiEventBroadcaster, PlayerInventoryEmitsPin) {
  /*
   * Given a GUI client and a freshly spawned drone (10 food, no minerals)
   * When playerInventory is broadcast
   * Then the client receives "pin #1 5 6 10 0 0 0 0 0 0"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);
  const Player& player =
      harness.players.spawn("red", 5, 6, Direction::North, harness.map);

  harness.gui.playerInventory(player);
  harness.drive();

  EXPECT_EQ(client.receive(), "pin #1 5 6 10 0 0 0 0 0 0\n");
}

TEST(GuiEventBroadcaster, PlayerDiedEmitsPdi) {
  /*
   * Given a GUI client
   * When playerDied is broadcast for drone 1
   * Then the client receives "pdi #1"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.playerDied(1);
  harness.drive();

  EXPECT_EQ(client.receive(), "pdi #1\n");
}

TEST(GuiEventBroadcaster, ResourceCollectedEmitsPgt) {
  /*
   * Given a GUI client
   * When resourceCollected is broadcast for drone 1 taking linemate
   * Then the client receives "pgt #1 1"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.resourceCollected(1, ResourceType::Linemate);
  harness.drive();

  EXPECT_EQ(client.receive(), "pgt #1 1\n");
}

TEST(GuiEventBroadcaster, ResourceDroppedEmitsPdr) {
  /*
   * Given a GUI client
   * When resourceDropped is broadcast for drone 1 setting down sibur
   * Then the client receives "pdr #1 3"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.resourceDropped(1, ResourceType::Sibur);
  harness.drive();

  EXPECT_EQ(client.receive(), "pdr #1 3\n");
}

TEST(GuiEventBroadcaster, TileChangedEmitsBct) {
  /*
   * Given a GUI client and a tile (1, 1) holding one linemate
   * When tileChanged is broadcast for that tile
   * Then the client receives "bct 1 1 0 1 0 0 0 0 0"
   */
  BroadcasterHarness harness(4, 4);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);
  harness.map.tileAt(1, 1).drop(ResourceType::Linemate);

  harness.gui.tileChanged(1, 1, harness.map.tileAt(1, 1));
  harness.drive();

  EXPECT_EQ(client.receive(), "bct 1 1 0 1 0 0 0 0 0\n");
}

TEST(GuiEventBroadcaster, BroadcastsToEveryGuiClient) {
  /*
   * Given two connected GUI clients
   * When playerDied is broadcast once
   * Then both clients receive "pdi #1"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient first(harness.server.port());
  handshake(harness, first, graphicTeam);
  LoopbackClient second(harness.server.port());
  handshake(harness, second, graphicTeam);

  harness.gui.playerDied(1);
  harness.drive();

  EXPECT_EQ(first.receive(), "pdi #1\n");
  EXPECT_EQ(second.receive(), "pdi #1\n");
}

TEST(GuiEventBroadcaster, SkipsAiSessions) {
  /*
   * Given an AI session connected alongside a GUI client
   * When an event is broadcast and the AI session then sends an unknown command
   * Then the AI session's first reply is the unknown sentinel, not the event
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient gui(harness.server.port());
  handshake(harness, gui, graphicTeam);
  LoopbackClient aiClient(harness.server.port());
  handshake(harness, aiClient, "red");

  harness.gui.playerDied(1);
  harness.drive();
  aiClient.sendLine("whatever");
  harness.drive();

  EXPECT_EQ(gui.receive(), "pdi #1\n");
  EXPECT_EQ(aiClient.receive(), "sentinel\n");
}

TEST(GuiEventBroadcaster, PlayerBroadcastEmitsPbc) {
  /*
   * Given a GUI client
   * When playerBroadcast is sent for drone 1 with text "hello world"
   * Then the client receives "pbc #1 hello world"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.playerBroadcast(1, "hello world");
  harness.drive();

  EXPECT_EQ(client.receive(), "pbc #1 hello world\n");
}

TEST(GuiEventBroadcaster, PlayerExpelledEmitsPex) {
  /*
   * Given a GUI client
   * When playerExpelled is sent for drone 2
   * Then the client receives "pex #2"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.playerExpelled(2);
  harness.drive();

  EXPECT_EQ(client.receive(), "pex #2\n");
}

TEST(GuiEventBroadcaster, IncantationStartedEmitsPic) {
  /*
   * Given a GUI client
   * When incantationStarted is sent at (3, 4) level 2 with drones 1 and 2
   * Then the client receives "pic 3 4 2 #1 #2"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.incantationStarted(3, 4, 2, {1, 2});
  harness.drive();

  EXPECT_EQ(client.receive(), "pic 3 4 2 #1 #2\n");
}

TEST(GuiEventBroadcaster, IncantationEndedEmitsPie) {
  /*
   * Given a GUI client
   * When incantationEnded is sent at (3, 4) with success
   * Then the client receives "pie 3 4 1"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.incantationEnded(3, 4, true);
  harness.drive();

  EXPECT_EQ(client.receive(), "pie 3 4 1\n");
}

TEST(GuiEventBroadcaster, EggLaidEmitsPfkThenEnw) {
  /*
   * Given a GUI client and an egg #7 laid by drone 1 at (2, 5)
   * When eggLaid is broadcast
   * Then the client receives "pfk #1" then "enw #7 #1 2 5"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);
  const Egg egg{.id = 7, .teamName = "red", .x = 2, .y = 5};

  harness.gui.eggLaid(1, egg);
  harness.drive();

  EXPECT_EQ(client.receive(), "pfk #1\nenw #7 #1 2 5\n");
}

TEST(GuiEventBroadcaster, EggHatchedEmitsEbo) {
  /*
   * Given a GUI client
   * When eggHatched is sent for egg 7
   * Then the client receives "ebo #7"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.eggHatched(7);
  harness.drive();

  EXPECT_EQ(client.receive(), "ebo #7\n");
}

TEST(GuiEventBroadcaster, EggDestroyedEmitsEdi) {
  /*
   * Given a GUI client
   * When eggDestroyed is sent for egg 7
   * Then the client receives "edi #7"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.eggDestroyed(7);
  harness.drive();

  EXPECT_EQ(client.receive(), "edi #7\n");
}

TEST(GuiEventBroadcaster, EndGameEmitsSeg) {
  /*
   * Given a GUI client
   * When endGame is broadcast for team "red"
   * Then the client receives "seg red"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.endGame("red");
  harness.drive();

  EXPECT_EQ(client.receive(), "seg red\n");
}

TEST(GuiEventBroadcaster, ServerMessageEmitsSmg) {
  /*
   * Given a GUI client
   * When serverMessage is broadcast with "shutting down"
   * Then the client receives "smg shutting down"
   */
  BroadcasterHarness harness(mapSide, mapSide);
  LoopbackClient client(harness.server.port());
  handshake(harness, client, graphicTeam);

  harness.gui.serverMessage("shutting down");
  harness.drive();

  EXPECT_EQ(client.receive(), "smg shutting down\n");
}
