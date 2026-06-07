/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** OptionSchema — declarative description of every server command-line option
*/

#include "Cli/Validators/OptionSchema.hpp"
#include <charconv>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Schema/Fields/BoundedNumberFieldType.hpp"
#include "Schema/Fields/StringFieldType.hpp"

namespace zappy::server {

namespace {

int toInteger(const std::string &value) {
  int result = 0;
  const char *begin = value.data();
  std::from_chars(begin, begin + value.size(), result);
  return result;
}

std::shared_ptr<zappy::schema::IFieldType> boundedNumber(std::string name,
                                                         std::int64_t minimum,
                                                         std::int64_t maximum) {
  return std::make_shared<zappy::schema::BoundedNumberFieldType>(
      std::move(name), minimum, maximum);
}

OptionApplier integerSetter(int ServerConfig::*field) {
  return [field](ServerConfig &config, const std::vector<std::string> &values) {
    config.*field = toInteger(values.front());
  };
}

void applyTeamNames(ServerConfig &config,
                    const std::vector<std::string> &values) {
  config.teamNames = values;
}

}  // namespace

std::vector<OptionSchema> buildServerOptionSchema() {
  constexpr std::int64_t maxInteger = std::numeric_limits<int>::max();
  constexpr std::int64_t maxPort = 65535;
  return {
      {.flag = "-p",
       .description = "TCP port to listen on",
       .arity = OptionArity::SingleValue,
       .fieldType = boundedNumber("port", 1, maxPort),
       .apply = integerSetter(&ServerConfig::port)},
      {.flag = "-x",
       .description = "map width",
       .arity = OptionArity::SingleValue,
       .fieldType = boundedNumber("width", 1, maxInteger),
       .apply = integerSetter(&ServerConfig::width)},
      {.flag = "-y",
       .description = "map height",
       .arity = OptionArity::SingleValue,
       .fieldType = boundedNumber("height", 1, maxInteger),
       .apply = integerSetter(&ServerConfig::height)},
      {.flag = "-c",
       .description = "clients allowed per team",
       .arity = OptionArity::SingleValue,
       .fieldType = boundedNumber("clientsPerTeam", 1, maxInteger),
       .apply = integerSetter(&ServerConfig::clientsPerTeam)},
      {.flag = "-f",
       .description = "reciprocal of the time unit",
       .arity = OptionArity::SingleValue,
       .fieldType = boundedNumber("frequency", 1, maxInteger),
       .apply = integerSetter(&ServerConfig::frequency)},
      {.flag = "-n",
       .description = "team names",
       .arity = OptionArity::ValueList,
       .fieldType =
           std::make_shared<zappy::schema::StringFieldType>("teamName"),
       .apply = applyTeamNames},
  };
}

}  // namespace zappy::server
