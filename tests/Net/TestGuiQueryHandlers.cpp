/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Integration tests for installGuiQueryHandlers
*/

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "App/ServerConfig.hpp"
#include "App/TimeUnit.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "App/World/Resources/ResourceType.hpp"
#include "LoopbackClient.hpp"
#include "Net/ClientContext.hpp"
#include "Net/GuiQueryHandlers.hpp"
#include "Protocol/GuiProtocol.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Server/RPCServer.hpp"
#include "Rpc/Session/Session.hpp"

using zappy::protocol::BadParameter;
using zappy::protocol::UnknownCommand;
using zappy::rpc::IMessage;
using zappy::rpc::RPCServer;
using zappy::rpc::Session;
using zappy::rpc::test::LoopbackClient;
using zappy::server::ClientContext;
using zappy::server::ClientType;
using zappy::server::installGuiQueryHandlers;
using zappy::server::ServerConfig;
using zappy::server::TimeUnit;
using zappy::world::Direction;
using zappy::world::Map;
using zappy::world::PlayerRegistry;
using zappy::world::ResourceType;

namespace {

constexpr int driveIterations = 4;
constexpr int driveTimeoutMs = 20;

constexpr int harnessFrequency = 100;

struct GuiHarness {
  ServerConfig config;
  Map map;
  PlayerRegistry players;
  TimeUnit timeUnit;
  RPCServer<ClientContext> server;

  GuiHarness(int width, int height, std::vector<std::string> teams)
      : config{.port = 0,
               .width = width,
               .height = height,
               .clientsPerTeam = 1,
               .frequency = harnessFrequency,
               .teamNames = std::move(teams)},
        map(width, height),
        timeUnit(harnessFrequency),
        server(0) {
    server.onHandshake([](Session<ClientContext>& session, const std::string&) {
      session.ctx().type = ClientType::Gui;
      session.completeHandshake();
    });
    server.onUnknown([](Session<ClientContext>& session, IMessage&) {
      session.send(UnknownCommand().opcode());
    });
    server.onInvalid([](Session<ClientContext>& session, IMessage&) {
      session.send(BadParameter().opcode());
    });
    installGuiQueryHandlers(server, config, map, players, timeUnit);
    server.start();
  }

  void drive(int iterations = driveIterations) {
    for (int index = 0; index < iterations; index++) {
      server.runOnce(driveTimeoutMs);
    }
  }
};

void completeHandshake(LoopbackClient& client) { client.sendLine("GRAPHIC"); }

std::string collectLine(LoopbackClient& client) { return client.receive(); }

}  // namespace

TEST(GuiQueryHandlers, MszReturnsConfiguredDimensions) {
  /*
   * Given a server configured with width 12 and height 8
   * When a GUI client sends msz
   * Then it receives "msz 12 8"
   */
  GuiHarness harness(12, 8, {});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("msz");
  harness.drive();
  EXPECT_EQ(collectLine(client), "msz 12 8\n");
}

TEST(GuiQueryHandlers, TnaEmitsOneLinePerTeam) {
  /*
   * Given a server configured with two team names
   * When a GUI client sends tna
   * Then it receives one tna line per team in configured order
   */
  GuiHarness harness(4, 4, {"red", "blue"});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("tna");
  harness.drive();
  EXPECT_EQ(collectLine(client), "tna red\ntna blue\n");
}

TEST(GuiQueryHandlers, TnaProducesNothingWhenNoTeam) {
  /*
   * Given a server with no configured team
   * When a GUI client sends tna
   * Then it receives no output (queue stays silent)
   */
  GuiHarness harness(4, 4, {});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("tna");
  harness.drive();
  EXPECT_EQ(collectLine(client), "");
}

TEST(GuiQueryHandlers, BctSerializesEmptyTileWithZeroes) {
  /*
   * Given a freshly built 4x4 map (every tile empty)
   * When a GUI client requests bct 1 2
   * Then it receives "bct 1 2 0 0 0 0 0 0 0"
   */
  GuiHarness harness(4, 4, {});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("bct 1 2");
  harness.drive();
  EXPECT_EQ(collectLine(client), "bct 1 2 0 0 0 0 0 0 0\n");
}

TEST(GuiQueryHandlers, BctSerializesResourcesInProtocolOrder) {
  /*
   * Given a tile with one of each resource kind plus two food
   * When a GUI client requests bct 0 0
   * Then it receives "bct 0 0 2 1 1 1 1 1 1" (food first, then 6 minerals)
   */
  GuiHarness harness(4, 4, {});
  harness.map.tileAt(0, 0).drop(ResourceType::Food, 2);
  harness.map.tileAt(0, 0).drop(ResourceType::Linemate);
  harness.map.tileAt(0, 0).drop(ResourceType::Deraumere);
  harness.map.tileAt(0, 0).drop(ResourceType::Sibur);
  harness.map.tileAt(0, 0).drop(ResourceType::Mendiane);
  harness.map.tileAt(0, 0).drop(ResourceType::Phiras);
  harness.map.tileAt(0, 0).drop(ResourceType::Thystame);

  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("bct 0 0");
  harness.drive();
  EXPECT_EQ(collectLine(client), "bct 0 0 2 1 1 1 1 1 1\n");
}

TEST(GuiQueryHandlers, BctRejectsOverflowingCoordinatesWithSbp) {
  /*
   * Given a coordinate that parses as a number but overflows int
   * When a GUI client sends bct with that value
   * Then the server replies sbp instead of throwing out_of_range
   */
  GuiHarness harness(4, 4, {});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("bct 99999999999999999999 0");
  harness.drive();
  EXPECT_EQ(collectLine(client), "sbp\n");
}

TEST(GuiQueryHandlers, BctRejectsNonNumericCoordinatesWithSbp) {
  /*
   * Given the GUI invalid-parameter convention (sbp)
   * When a GUI client sends bct with garbage coordinates
   * Then the server replies sbp (schema validation fails)
   */
  GuiHarness harness(4, 4, {});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("bct foo bar");
  harness.drive();
  EXPECT_EQ(collectLine(client), "sbp\n");
}

TEST(GuiQueryHandlers, BctWrapsOutOfRangeCoordinatesToroidally) {
  /*
   * Given the toroidal map semantics of world::Map::tileAt
   * When a GUI client requests bct -1 -1 on a 4x4 map
   * Then it receives the cell at (3, 3) but echoes the requested coordinates
   */
  GuiHarness harness(4, 4, {});
  harness.map.tileAt(3, 3).drop(ResourceType::Food, 5);
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("bct -1 -1");
  harness.drive();
  EXPECT_EQ(collectLine(client), "bct -1 -1 5 0 0 0 0 0 0\n");
}

TEST(GuiQueryHandlers, MctEmitsOneBctPerCell) {
  /*
   * Given a 3x2 map with one resource on tile (2, 1)
   * When a GUI client sends mct
   * Then it receives six bct lines in row-major order, that tile carrying 1
   *      linemate
   */
  GuiHarness harness(3, 2, {});
  harness.map.tileAt(2, 1).drop(ResourceType::Linemate);
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("mct");
  harness.drive();
  const std::string expected =
      "bct 0 0 0 0 0 0 0 0 0\n"
      "bct 1 0 0 0 0 0 0 0 0\n"
      "bct 2 0 0 0 0 0 0 0 0\n"
      "bct 0 1 0 0 0 0 0 0 0\n"
      "bct 1 1 0 0 0 0 0 0 0\n"
      "bct 2 1 0 1 0 0 0 0 0\n";
  EXPECT_EQ(collectLine(client), expected);
}

TEST(GuiQueryHandlers, UnknownGuiOpcodeRepliesSuc) {
  /*
   * Given the GUI unknown-command convention (suc)
   * When a GUI client sends a garbage opcode
   * Then the server replies suc via onUnknown
   */
  GuiHarness harness(4, 4, {});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("zzz");
  harness.drive();
  EXPECT_EQ(collectLine(client), "suc\n");
}

TEST(GuiQueryHandlers, PpoReturnsPositionAndOrientation) {
  /*
   * Given a drone spawned at (2, 3) facing East (orientation 2)
   * When a GUI client requests ppo #1
   * Then it receives "ppo #1 2 3 2"
   */
  GuiHarness harness(8, 8, {"red"});
  harness.players.spawn("red", 2, 3, Direction::East, harness.map);
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("ppo #1");
  harness.drive();
  EXPECT_EQ(collectLine(client), "ppo #1 2 3 2\n");
}

TEST(GuiQueryHandlers, PlvReturnsPlayerLevel) {
  /*
   * Given a drone elevated to level 4
   * When a GUI client requests plv #1
   * Then it receives "plv #1 4"
   */
  GuiHarness harness(8, 8, {"red"});
  harness.players.spawn("red", 0, 0, Direction::North, harness.map);
  harness.players.find(1)->setLevel(4);
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("plv #1");
  harness.drive();
  EXPECT_EQ(collectLine(client), "plv #1 4\n");
}

TEST(GuiQueryHandlers, PinReturnsPositionThenInventory) {
  /*
   * Given a drone at (1, 1) carrying its starting food and one linemate
   * When a GUI client requests pin #1
   * Then it receives "pin #1 1 1 10 1 0 0 0 0 0"
   */
  GuiHarness harness(8, 8, {"red"});
  harness.players.spawn("red", 1, 1, Direction::North, harness.map);
  harness.players.find(1)->inventory().add(ResourceType::Linemate);
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("pin #1");
  harness.drive();
  EXPECT_EQ(collectLine(client), "pin #1 1 1 10 1 0 0 0 0 0\n");
}

TEST(GuiQueryHandlers, PlayerQueryForUnknownDroneRepliesSbp) {
  /*
   * Given a world with no drone numbered 9
   * When a GUI client requests ppo #9
   * Then the server replies sbp
   */
  GuiHarness harness(8, 8, {"red"});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("ppo #9");
  harness.drive();
  EXPECT_EQ(collectLine(client), "sbp\n");
}

TEST(GuiQueryHandlers, PlayerQueryWithoutHashRepliesSbp) {
  /*
   * Given the GUI player-number convention (#n)
   * When a GUI client requests ppo 1 without the leading hash
   * Then the server replies sbp
   */
  GuiHarness harness(8, 8, {"red"});
  harness.players.spawn("red", 0, 0, Direction::North, harness.map);
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("ppo 1");
  harness.drive();
  EXPECT_EQ(collectLine(client), "sbp\n");
}

TEST(GuiQueryHandlers, SgtReturnsTheCurrentTimeUnit) {
  /*
   * Given a server started at frequency 100
   * When a GUI client sends sgt
   * Then it receives "sgt 100"
   */
  GuiHarness harness(4, 4, {});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("sgt");
  harness.drive();
  EXPECT_EQ(collectLine(client), "sgt 100\n");
}

TEST(GuiQueryHandlers, SstUpdatesTheTimeUnitAndEchoesIt) {
  /*
   * Given a running server
   * When a GUI client sends sst 250
   * Then it receives "sst 250" and the shared time unit becomes 250
   */
  GuiHarness harness(4, 4, {});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("sst 250");
  harness.drive();
  EXPECT_EQ(collectLine(client), "sst 250\n");
  EXPECT_EQ(harness.timeUnit.value(), 250);
}

TEST(GuiQueryHandlers, SstRejectsNonPositiveTimeUnitWithSbp) {
  /*
   * Given the rule that the time unit must stay strictly positive
   * When a GUI client sends sst 0
   * Then the server replies sbp and the time unit is unchanged
   */
  GuiHarness harness(4, 4, {});
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("sst 0");
  harness.drive();
  EXPECT_EQ(collectLine(client), "sbp\n");
  EXPECT_EQ(harness.timeUnit.value(), harnessFrequency);
}
