/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Integration tests for the GameServer AI movement and observation commands
*/

#include <gtest/gtest.h>
#include <string>
#include "App/GameServer.hpp"
#include "App/ServerConfig.hpp"
#include "LoopbackClient.hpp"

using zappy::rpc::test::LoopbackClient;
using zappy::server::GameServer;
using zappy::server::ServerConfig;

namespace {

constexpr int handshakeIterations = 3;
constexpr int actionIterations = 12;

void drive(GameServer& server, int iterations) {
  for (int index = 0; index < iterations; index++) {
    server.runOnce(50);
  }
}

ServerConfig makeConfig(int slotsPerTeam) {
  ServerConfig config;
  config.port = 0;
  config.width = 7;
  config.height = 7;
  config.clientsPerTeam = slotsPerTeam;
  config.frequency = 100;
  config.teamNames = {"red"};
  return config;
}

void joinAi(GameServer& server, LoopbackClient& client) {
  drive(server, 2);
  client.receive();
  client.sendLine("red");
  drive(server, handshakeIterations);
  client.receive();
}

std::string runCommand(GameServer& server, LoopbackClient& client,
                       const std::string& command) {
  client.sendLine(command);
  drive(server, actionIterations);
  return client.receive();
}

}  // namespace

TEST(GameServerAiCommands, ForwardRepliesOk) {
  /*
   * Given an authenticated AI drone
   * When it sends Forward
   * Then the server acknowledges the move with a newline-terminated ok
   */
  GameServer server(makeConfig(2));
  server.start();
  LoopbackClient client(server.port());
  joinAi(server, client);

  EXPECT_EQ(runCommand(server, client, "Forward"), "ok\n");
}

TEST(GameServerAiCommands, RightRepliesOk) {
  /*
   * Given an authenticated AI drone
   * When it sends Right
   * Then the server acknowledges the turn with ok
   */
  GameServer server(makeConfig(2));
  server.start();
  LoopbackClient client(server.port());
  joinAi(server, client);

  EXPECT_EQ(runCommand(server, client, "Right"), "ok\n");
}

TEST(GameServerAiCommands, InventoryReportsTheStartingFoodReserve) {
  /*
   * Given a freshly spawned AI drone
   * When it sends Inventory
   * Then the seven resources are reported with the initial ten food units
   */
  GameServer server(makeConfig(2));
  server.start();
  LoopbackClient client(server.port());
  joinAi(server, client);

  EXPECT_EQ(runCommand(server, client, "Inventory"),
            "[food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, "
            "phiras 0, thystame 0]\n");
}

TEST(GameServerAiCommands, LookStartsWithThePlayersOwnTile) {
  /*
   * Given an AI drone standing on its tile
   * When it sends Look
   * Then the response opens on its own tile, which carries the player token
   */
  GameServer server(makeConfig(2));
  server.start();
  LoopbackClient client(server.port());
  joinAi(server, client);

  const std::string look = runCommand(server, client, "Look");
  EXPECT_EQ(look.rfind("[player", 0), 0U);
  EXPECT_EQ(look.back(), '\n');
}

TEST(GameServerAiCommands, ConnectNbrReportsRemainingTeamSlots) {
  /*
   * Given a two-slot team with one drone already connected
   * When that drone sends Connect_nbr
   * Then the server replies with the one remaining free slot
   */
  GameServer server(makeConfig(2));
  server.start();
  LoopbackClient client(server.port());
  joinAi(server, client);

  EXPECT_EQ(runCommand(server, client, "Connect_nbr"), "1\n");
}

TEST(GameServerAiCommands, TakeUnknownObjectRepliesKo) {
  /*
   * Given an authenticated AI drone
   * When it asks to take an object that names no resource
   * Then the server rejects the command with ko without dropping the session
   */
  GameServer server(makeConfig(2));
  server.start();
  LoopbackClient client(server.port());
  joinAi(server, client);

  EXPECT_EQ(runCommand(server, client, "Take wood"), "ko\n");
  EXPECT_EQ(runCommand(server, client, "Right"), "ok\n");
}

TEST(GameServerAiCommands, UnknownCommandRepliesKo) {
  /*
   * Given an authenticated AI drone
   * When it sends a command outside the protocol
   * Then the server replies ko and keeps serving the session
   */
  GameServer server(makeConfig(2));
  server.start();
  LoopbackClient client(server.port());
  joinAi(server, client);

  EXPECT_EQ(runCommand(server, client, "Nonsense"), "ko\n");
  EXPECT_EQ(runCommand(server, client, "Forward"), "ok\n");
}
