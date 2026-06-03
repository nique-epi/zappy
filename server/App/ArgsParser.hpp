/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgsParser — turns argv into a ServerConfig
*/

#pragma once

#include "App/ServerConfig.hpp"

namespace zappy::server {

/**
 * @brief Parse the server command line into a ServerConfig.
 *
 * Recognises -p, -x, -y, -c, -f (single integer values) and -n (one or more
 * team names until the next option). Unrecognised flags are rejected.
 *
 * @param argumentCount Number of entries in @p arguments (argc).
 * @param arguments     Argument vector (argv).
 * @returns The populated configuration.
 * @throws std::invalid_argument On a malformed or incomplete option.
 */
ServerConfig parseArguments(int argumentCount, char **arguments);

}  // namespace zappy::server
