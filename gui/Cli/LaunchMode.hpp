/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** LaunchMode — how the GUI was launched (CLI connection arguments)
*/

#pragma once

namespace zappy::gui {

/**
 * @brief Whether both connection flags (-p and -h) are present.
 *
 * Decides between a direct CLI connection and falling back to the menu. Does
 * not validate the flag values — that is @ref parseArguments' job.
 */
bool hasConnectionArguments(int argumentCount, char** arguments);

}  // namespace zappy::gui
