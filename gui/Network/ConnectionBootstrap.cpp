/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ConnectionBootstrap — handshake then world initialization over the network
*/

#include "Network/ConnectionBootstrap.hpp"
#include <raylib.h>
#include <string>
#include "Network/NetworkManager.hpp"
#include "Network/Parsing/MessageParser.hpp"
#include "Network/ServerHandshake.hpp"
#include "World/WorldInitializer.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace {

constexpr int STARTUP_POLL_TIMEOUT_MS = 50;

}  // namespace

bool handshakeAndInitializeWorld(NetworkManager& network, WorldState& world,
                                 MessageParser& parser) {
  const ServerHandshake handshake(network);
  while (handshake.status() != HandshakeStatus::Done) {
    if (WindowShouldClose()) {
      return false;
    }
    network.runOnce(STARTUP_POLL_TIMEOUT_MS);
    handshake.checkTimeout();
  }

  {
    WorldInitializer initializer(network, parser, world);
    while (!initializer.isDone()) {
      if (WindowShouldClose()) {
        return false;
      }
      network.runOnce(STARTUP_POLL_TIMEOUT_MS);
      initializer.checkTimeout();
      initializer.onPollRoundComplete();
    }
  }

  network.setResponseHandler(
      [&parser](const std::string& line) { parser.parseLine(line); });
  return true;
}

}  // namespace zappy::gui
