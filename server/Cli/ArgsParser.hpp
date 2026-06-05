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
 * The recognised options and their accepted ranges are described by
 * @ref buildServerOptionSchema: -p, -x, -y, -c, -f take a single bounded
 * integer and -n takes one or more team names until the next option. At least
 * one team name is required. Every failure surfaces as a @ref ParserException
 * subclass; no standard-library exception escapes.
 *
 * @param argumentCount Number of entries in @p arguments (argc).
 * @param arguments     Argument vector (argv).
 * @returns The populated configuration.
 * @throws UnknownOptionException On an unrecognised flag.
 * @throws MissingValueException  When a required value is absent.
 * @throws InvalidValueException  When a value is not a valid in-range integer.
 */
ServerConfig parseArguments(int argumentCount, char **arguments);

}  // namespace zappy::server
