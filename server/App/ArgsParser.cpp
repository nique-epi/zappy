/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgsParser — turns argv into a ServerConfig
*/

#include "App/ArgsParser.hpp"
#include <stdexcept>
#include <string>

namespace zappy::server {

namespace {

int requireInteger(int argumentCount, char **arguments, int &index,
                   const std::string &option) {
  if (index + 1 >= argumentCount) {
    throw std::invalid_argument("missing value for " + option);
  }
  index += 1;
  return std::stoi(arguments[index]);
}

bool isOption(const char *token) { return token[0] == '-' && token[1] != '\0'; }

}  // namespace

ServerConfig parseArguments(int argumentCount, char **arguments) {
  ServerConfig config;

  for (int index = 1; index < argumentCount; ++index) {
    const std::string option = arguments[index];
    if (option == "-p") {
      config.port = requireInteger(argumentCount, arguments, index, option);
    } else if (option == "-x") {
      config.width = requireInteger(argumentCount, arguments, index, option);
    } else if (option == "-y") {
      config.height = requireInteger(argumentCount, arguments, index, option);
    } else if (option == "-c") {
      config.clientsPerTeam =
          requireInteger(argumentCount, arguments, index, option);
    } else if (option == "-f") {
      config.frequency =
          requireInteger(argumentCount, arguments, index, option);
    } else if (option == "-n") {
      while (index + 1 < argumentCount && !isOption(arguments[index + 1])) {
        index += 1;
        config.teamNames.emplace_back(arguments[index]);
      }
    } else {
      throw std::invalid_argument("unknown option " + option);
    }
  }
  return config;
}

}  // namespace zappy::server
