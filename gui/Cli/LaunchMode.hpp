/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** LaunchMode — how the GUI was launched (help, CLI connection arguments)
*/

#pragma once

#include <string_view>

namespace zappy::gui {

/**
 * @brief Usage line printed for --help and on a command-line error.
 */
inline constexpr std::string_view GUI_USAGE =
    "USAGE: ./zappy_gui -p port -h machine";

/**
 * @brief Whether the user passed --help anywhere on the command line.
 */
bool helpCommand(int argumentCount, char** arguments);

/**
 * @brief Whether both connection flags (-p and -h) are present.
 *
 * Decides between a direct CLI connection and falling back to the menu. Does
 * not validate the flag values — that is @ref parseArguments' job.
 */
bool hasConnectionArguments(int argumentCount, char** arguments);

}  // namespace zappy::gui
