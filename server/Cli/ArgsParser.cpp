/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgsParser — turns argv into a ServerConfig
*/

#include "Cli/ArgsParser.hpp"
#include <string>
#include "Cli/Exceptions/ParserException.hpp"
#include "Cli/OptionSchema.hpp"
#include "Cli/UsageHelp.hpp"
#include "Cli/Validators/OptionSchema.hpp"

namespace zappy::server {

namespace {

constexpr const char* usageSynopsis =
    "./zappy_server -p port -x width -y height -n name1 name2 ... "
    "-c clientsNb -f freq";

zappy::cli::UsageHelp<ServerConfig> serverUsageHelp() {
  return {usageSynopsis, buildServerOptionSchema()};
}

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

bool helpRequested(int argumentCount, char** arguments) {
  return serverUsageHelp().isRequested(argumentCount, arguments);
}

std::string usageMessage() { return serverUsageHelp().text(); }

}  // namespace zappy::server
