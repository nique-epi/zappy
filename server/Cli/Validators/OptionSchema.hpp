/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** OptionSchema — declarative description of every server command-line option
*/

#pragma once

#include <vector>
#include "App/ServerConfig.hpp"
#include "Cli/OptionSchema.hpp"

namespace zappy::server {

using OptionSchema = zappy::cli::OptionSchema<ServerConfig>;

using OptionApplier =
    std::function<void(ServerConfig&, const std::vector<std::string>&)>;

/**
 * @brief Build the schema describing every recognised server option.
 *
 * Integer options validate with a bounded-number field type carrying their
 * accepted range; `-n` validates each team name as a non-empty string and
 * collects every following token up to the next flag.
 *
 * @returns The ordered list of option schemas.
 */
std::vector<OptionSchema> buildServerOptionSchema();

}  // namespace zappy::server
