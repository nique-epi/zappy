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
#include "App/World/Map/Map.hpp"
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
using zappy::world::Map;
using zappy::world::ResourceType;

namespace {

constexpr int driveIterations = 4;
constexpr int driveTimeoutMs = 20;

struct GuiHarness {
  ServerConfig config;
  Map map;
  RPCServer<ClientContext> server;

  GuiHarness(int width, int height, std::vector<std::string> teams)
      : config{.port = 0,
               .width = width,
               .height = height,
               .clientsPerTeam = 1,
               .frequency = 100,
               .teamNames = std::move(teams)},
        map(width, height),
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
    installGuiQueryHandlers(server, config, map);
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
