/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgsParser — turns argv into a ServerConfig
*/

#include "Cli/ArgsParser.hpp"
#include "Cli/Exceptions/ParserException.hpp"
#include "Cli/OptionSchema.hpp"
#include "Cli/Validators/OptionSchema.hpp"

namespace zappy::server {

namespace {

void validateConfig(const ServerConfig& config) {
  if (config.teamNames.empty()) {
    throw zappy::cli::MissingValueException("-n");
  }
}

}  // namespace

ServerConfig parseArguments(int argumentCount, char** arguments) {
  return zappy::cli::parseWithSchema<ServerConfig>(
      argumentCount, arguments, buildServerOptionSchema(), validateConfig);
}

}  // namespace zappy::server
