/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Integration tests for WorldInitializer against a real GameServer over
* loopback
*/

#include <gtest/gtest.h>
#include "App/GameServer.hpp"
#include "App/ServerConfig.hpp"
#include "Network/NetworkManager.hpp"
#include "Network/Parsing/MessageParser.hpp"
#include "Network/ServerHandshake.hpp"
#include "World/WorldInitializer.hpp"
#include "World/WorldState.hpp"

using zappy::server::GameServer;
using zappy::server::ServerConfig;

namespace {

constexpr int pollTimeoutMs = 10;
constexpr int maxIterations = 200;

ServerConfig makeConfig() {
  ServerConfig config;
  config.port = 0;
  config.width = 3;
  config.height = 2;
  config.clientsPerTeam = 1;
  config.frequency = 100;
  config.teamNames = {"red", "blue"};
  return config;
}

void completeHandshake(GameServer& server, zappy::gui::NetworkManager& network,
                       zappy::gui::ServerHandshake& handshake) {
  for (int index = 0; index < maxIterations &&
                      handshake.status() != zappy::gui::HandshakeStatus::Done;
       index++) {
    server.runOnce(pollTimeoutMs);
    network.runOnce(pollTimeoutMs);
    handshake.checkTimeout();
  }
}

void completeWorldInit(GameServer& server, zappy::gui::NetworkManager& network,
                       zappy::gui::WorldInitializer& initializer) {
  for (int index = 0; index < maxIterations && !initializer.isDone(); index++) {
    server.runOnce(pollTimeoutMs);
    network.runOnce(pollTimeoutMs);
    initializer.checkTimeout();
    initializer.onPollRoundComplete();
  }
}

}  // namespace

TEST(WorldInitializerIntegration, PopulatesWorldStateFromRealServer) {
  /*
   * Given a real GameServer listening on loopback with a 3x2 map and two
   * teams
   * When a GUI client completes the handshake and runs WorldInitializer
   * against it
   * Then WorldState reflects the server's real map size, tile grid and team
   * names
   */
  GameServer server(makeConfig());
  server.start();

  zappy::gui::NetworkManager network("127.0.0.1", server.port());
  zappy::gui::ServerHandshake handshake(network);
  completeHandshake(server, network, handshake);
  ASSERT_EQ(handshake.status(), zappy::gui::HandshakeStatus::Done);

  zappy::gui::WorldState world;
  zappy::gui::MessageParser parser(world);
  zappy::gui::WorldInitializer initializer(network, parser, world);
  completeWorldInit(server, network, initializer);

  ASSERT_TRUE(initializer.isDone());
  EXPECT_EQ(world.width, 3);
  EXPECT_EQ(world.height, 2);
  ASSERT_EQ(world.tiles.size(), 2U);
  ASSERT_EQ(world.tiles[0].size(), 3U);
  EXPECT_EQ(world.tiles[1][2].x, 2);
  EXPECT_EQ(world.tiles[1][2].y, 1);
  ASSERT_EQ(world.teams.size(), 2U);
  EXPECT_EQ(world.teams[0], "red");
  EXPECT_EQ(world.teams[1], "blue");
}
