/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgsParser — turns argv into a GuiConfig
*/

#include "Cli/ArgsParser.hpp"
#include <charconv>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include "Cli/Exceptions/ParserException.hpp"
#include "Cli/OptionSchema.hpp"
#include "GuiConfig.hpp"
#include "Schema/Fields/BoundedNumberFieldType.hpp"
#include "Schema/Fields/StringFieldType.hpp"

namespace zappy::gui {

namespace {

std::vector<zappy::cli::OptionSchema<GuiConfig>> buildGuiOptionSchema() {
  constexpr std::int64_t maxPort = 65535;
  return {
      {.flag = "-p",
       .description = "server TCP port",
       .arity = zappy::cli::OptionArity::SingleValue,
       .fieldType = std::make_shared<zappy::schema::BoundedNumberFieldType>(
           "port", 1, maxPort),
       .apply =
           [](GuiConfig& config, const std::vector<std::string>& values) {
             int result = 0;
             const char* begin = values.front().data();
             std::from_chars(begin, begin + values.front().size(), result);
             config.port = result;
           }},
      {.flag = "-h",
       .description = "server hostname",
       .arity = zappy::cli::OptionArity::SingleValue,
       .fieldType =
           std::make_shared<zappy::schema::StringFieldType>("hostname"),
       .apply =
           [](GuiConfig& config, const std::vector<std::string>& values) {
             config.hostname = values.front();
           }},
  };
}

void validateGuiConfig(const GuiConfig& config) {
  if (config.port == 0) {
    throw zappy::cli::MissingValueException("-p");
  }
  if (config.hostname.empty()) {
    throw zappy::cli::MissingValueException("-h");
  }
}

}  // namespace

GuiConfig parseArguments(int argumentCount, char** arguments) {
  return zappy::cli::parseWithSchema<GuiConfig>(
      argumentCount, arguments, buildGuiOptionSchema(), validateGuiConfig);
}

}  // namespace zappy::gui
