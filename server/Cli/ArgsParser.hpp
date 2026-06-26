/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgsParser — turns argv into a ServerConfig
*/

#pragma once

#include <string>
#include "App/ServerConfig.hpp"

namespace zappy::server {

/**
 * @brief Whether the command line requests the usage message (`-h`/`--help`).
 *
 * Checked before @ref parseArguments so `./zappy_server --help` prints the
 * usage and exits successfully instead of failing on the missing `-n`.
 *
 * @param argumentCount Number of entries in @p arguments (argc).
 * @param arguments     Argument vector (argv).
 * @returns True when help was requested.
 */
bool helpRequested(int argumentCount, char **arguments);

/**
 * @brief The server `--help` usage text, with no trailing newline.
 *
 * @returns The formatted usage block describing every server option.
 */
std::string usageMessage();

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
