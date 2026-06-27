/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SessionConnector — establishes a server connection from CLI args or the menu
*/

#include "Network/SessionConnector.hpp"
#include <raylib.h>
#include <memory>
#include "Cli/ArgsParser.hpp"
#include "Menu/MenuScreen.hpp"
#include "Network/Exceptions.hpp"
#include "Network/NetworkManager.hpp"

namespace zappy::gui {

Session connectFromArguments(int argumentCount, char** arguments,
                             MenuScreen& menu) {
  auto config = parseArguments(argumentCount, arguments);
  try {
    return {.config = config,
            .network =
                std::make_unique<NetworkManager>(config.hostname, config.port)};
  } catch (const zappy::network::ConnectError& error) {
    menu.showConnectionError(error.what());
    return {};
  }
}

Session connectFromMenu(MenuScreen& menu) {
  while (!WindowShouldClose()) {
    auto config = menu.run();
    if (!config) {
      return {};
    }
    try {
      return {.config = config,
              .network = std::make_unique<NetworkManager>(config->hostname,
                                                          config->port)};
    } catch (const zappy::network::ConnectError& error) {
      menu.showConnectionError(error.what());
    }
  }
  return {};
}

}  // namespace zappy::gui
