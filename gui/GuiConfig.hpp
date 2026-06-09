/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiConfig — parsed command-line configuration of the Zappy GUI
*/

#pragma once

#include <string>

namespace zappy::gui {

/**
 * @brief Command-line configuration of the GUI.
 *
 * Populated by @ref parseArguments from the mandatory options:
 * -p port and -h hostname.
 */
struct GuiConfig {
  int port;
  std::string hostname;
};

}  // namespace zappy::gui
