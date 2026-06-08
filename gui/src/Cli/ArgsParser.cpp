/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgsParser — turns argv into a GuiConfig
*/

#include "Cli/ArgsParser.hpp"
#include <charconv>
#include <format>
#include <string>
#include "Cli/ArgumentCursor.hpp"
#include "Cli/Exceptions/ParserException.hpp"

namespace zappy::gui {

namespace {

constexpr int portMinimum = 1;
constexpr int portMaximum = 65535;

int parsePort(const std::string& value) {
  int result = 0;
  const char* begin = value.data();
  const char* end = begin + value.size();
  const auto [ptr, errorCode] = std::from_chars(begin, end, result);
  if (errorCode != std::errc{} || ptr != end || result < portMinimum ||
      result > portMaximum) {
    throw InvalidValueException(
        "-p", value,
        std::format("expected port in [{}, {}]", portMinimum, portMaximum));
  }
  return result;
}

}  // namespace

GuiConfig parseArguments(int argumentCount, char** arguments) {
  GuiConfig config{.port = 0, .hostname = {}};
  bool portProvided = false;
  bool hostnameProvided = false;

  ArgumentCursor cursor(argumentCount, arguments);
  while (cursor.hasNextToken()) {
    const std::string flag = cursor.nextToken();
    if (flag == "-p") {
      config.port = parsePort(cursor.requireValue("-p"));
      portProvided = true;
    } else if (flag == "-h") {
      config.hostname = cursor.requireValue("-h");
      hostnameProvided = true;
    } else {
      throw UnknownOptionException(flag);
    }
  }

  if (!portProvided) {
    throw MissingValueException("-p");
  }
  if (!hostnameProvided) {
    throw MissingValueException("-h");
  }
  return config;
}

}  // namespace zappy::gui
