/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Integration tests for the GameServer connection handshake over loopback
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

void drive(GameServer& server, int iterations) {
  for (int index = 0; index < iterations; index++) {
    server.runOnce(50);
  }
}

ServerConfig makeConfig(int slotsPerTeam) {
  ServerConfig config;
  config.port = 0;
  config.width = 5;
  config.height = 7;
  config.clientsPerTeam = slotsPerTeam;
  config.frequency = 100;
  config.teamNames = {"red"};
  return config;
}

void expectWelcome(GameServer& server, LoopbackClient& client) {
  drive(server, 2);
  EXPECT_EQ(client.receive(), "WELCOME\n");
}

}  // namespace

TEST(GameServerHandshake, SendsWelcomeOnConnect) {
  /*
   * Given a running server
   * When a client opens a connection
   * Then the server greets it with WELCOME before anything else
   */
  GameServer server(makeConfig(2));
  server.start();

  LoopbackClient client(server.port());
  drive(server, 2);

  EXPECT_EQ(client.receive(), "WELCOME\n");
}

TEST(GameServerHandshake, AiReceivesRemainingSlotsThenWorldSize) {
  /*
   * Given a team seeded with two slots
   * When an AI client sends the team name
   * Then the server replies with the remaining slot count then the world size
   */
  GameServer server(makeConfig(2));
  server.start();

  LoopbackClient client(server.port());
  expectWelcome(server, client);
  client.sendLine("red");
  drive(server, 3);

  EXPECT_EQ(client.receive(), "1\n5 7\n");
}

TEST(GameServerHandshake, FullTeamReceivesZeroSlotsAndAdmitsNoDrone) {
  /*
   * Given a team whose single slot is already taken by a first client
   * When a second client connects to the same full team
   * Then it receives 0 slots then the world size, no drone is admitted, and
   *      the server keeps serving later clients
   */
  GameServer server(makeConfig(1));
  server.start();

  LoopbackClient first(server.port());
  expectWelcome(server, first);
  first.sendLine("red");
  drive(server, 3);
  EXPECT_EQ(first.receive(), "0\n5 7\n");

  LoopbackClient second(server.port());
  expectWelcome(server, second);
  second.sendLine("red");
  drive(server, 3);
  EXPECT_EQ(second.receive(), "0\n5 7\n");

  LoopbackClient later(server.port());
  drive(server, 2);
  EXPECT_EQ(later.receive(), "WELCOME\n");
}

TEST(GameServerHandshake, GraphicClientConsumesNoSlotNorEgg) {
  /*
   * Given a team seeded with two slots
   * When a GRAPHIC client completes its handshake then an AI client joins the
   *      team
   * Then the GUI client gets no slot/world reply and the AI still sees both
   *      initial slots (minus its own), proving the GUI consumed no egg
   */
  GameServer server(makeConfig(2));
  server.start();

  LoopbackClient gui(server.port());
  expectWelcome(server, gui);
  gui.sendLine("GRAPHIC");
  drive(server, 3);
  EXPECT_EQ(gui.receive(), "");

  LoopbackClient aiClient(server.port());
  expectWelcome(server, aiClient);
  aiClient.sendLine("red");
  drive(server, 3);
  EXPECT_EQ(aiClient.receive(), "1\n5 7\n");
}
