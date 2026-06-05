/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgsParser — turns argv into a ServerConfig
*/

#include "Cli/ArgsParser.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include "Cli/ArgumentCursor.hpp"
#include "Cli/Exceptions/ParserException.hpp"
#include "Cli/Validators/OptionSchema.hpp"
#include "Schema/Fields/IFieldType.hpp"

namespace zappy::server {

namespace {

const OptionSchema &requireSchema(
    const std::unordered_map<std::string, const OptionSchema *> &byFlag,
    const std::string &flag) {
  const auto found = byFlag.find(flag);
  if (found == byFlag.end()) {
    throw UnknownOptionException(flag);
  }
  return *found->second;
}

void validateValue(const zappy::schema::IFieldType &fieldType,
                   const std::string &flag, const std::string &value) {
  if (!fieldType.validate(value)) {
    throw InvalidValueException(
        flag, value, std::string("expected ") + fieldType.typeName());
  }
}

std::vector<std::string> collectValues(const OptionSchema &option,
                                       ArgumentCursor &cursor) {
  std::vector<std::string> values;
  if (option.arity == OptionArity::SingleValue) {
    values.push_back(cursor.requireValue(option.flag));
  } else {
    while (cursor.nextIsValue()) {
      values.push_back(cursor.nextToken());
    }
    if (values.empty()) {
      throw MissingValueException(option.flag);
    }
  }
  for (const std::string &value : values) {
    validateValue(*option.fieldType, option.flag, value);
  }
  return values;
}

void validateConfig(const ServerConfig &config) {
  if (config.teamNames.empty()) {
    throw MissingValueException("-n");
  }
}

}  // namespace

ServerConfig parseArguments(int argumentCount, char **arguments) {
  const std::vector<OptionSchema> schema = buildServerOptionSchema();
  std::unordered_map<std::string, const OptionSchema *> byFlag;
  for (const OptionSchema &option : schema) {
    byFlag.emplace(option.flag, &option);
  }

  ServerConfig config;
  ArgumentCursor cursor(argumentCount, arguments);
  while (cursor.hasNextToken()) {
    const std::string flag = cursor.nextToken();
    const OptionSchema &option = requireSchema(byFlag, flag);
    option.apply(config, collectValues(option, cursor));
  }
  validateConfig(config);
  return config;
}

}  // namespace zappy::server
