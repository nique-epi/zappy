/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SessionConnector — establishes a server connection from CLI args or the menu
*/

#pragma once

#include <memory>
#include <optional>
#include "GuiConfig.hpp"
#include "Network/NetworkManager.hpp"

namespace zappy::gui {

class MenuScreen;

/**
 * @brief A connection attempt result: the chosen config and its live client.
 *
 * An empty @ref config means the user quit (no connection to run).
 */
struct Session {
  std::optional<GuiConfig> config;
  std::unique_ptr<NetworkManager> network;
};

/**
 * @brief Connect using the command-line -p/-h arguments.
 *
 * On a connection error, the message is shown in @p menu and an empty Session
 * is returned so the caller falls back to the menu.
 *
 * @throws zappy::cli::ParserException on an invalid command line.
 */
Session connectFromArguments(int argumentCount, char** arguments,
                             MenuScreen& menu);

/**
 * @brief Run the menu until the user connects successfully or quits.
 */
Session connectFromMenu(MenuScreen& menu);

}  // namespace zappy::gui
