/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** OptionSchema.tpp — template implementation of parseWithSchema
*/

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Cli/ArgumentCursor.hpp"
#include "Cli/Exceptions/ParserException.hpp"
#include "Error/Messages/ParserMessages.hpp"
#include "Schema/Fields/IFieldType.hpp"

namespace zappy::cli {

namespace detail {

inline void validateFieldValue(const zappy::schema::IFieldType& fieldType,
                               const std::string& flag,
                               const std::string& value) {
  if (!fieldType.validate(value)) {
    throw InvalidValueException(
        flag, value,
        std::string(zappy::error::messages::EXPECTED_PREFIX) +
            fieldType.typeName());
  }
}

template <typename Config>
std::vector<std::string> collectValues(const OptionSchema<Config>& option,
                                       ArgumentCursor& cursor) {
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
  for (const std::string& value : values) {
    validateFieldValue(*option.fieldType, option.flag, value);
  }
  return values;
}

}  // namespace detail

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
template <typename Config>
Config parseWithSchema(int argumentCount, char** arguments,
                       const std::vector<OptionSchema<Config>>& schema,
                       std::function<void(const Config&)> postValidate) {
  std::unordered_map<std::string, const OptionSchema<Config>*> byFlag;
  for (const OptionSchema<Config>& option : schema) {
    byFlag.emplace(option.flag, &option);
  }

  Config config{};
  ArgumentCursor cursor(argumentCount, arguments);
  while (cursor.hasNextToken()) {
    const std::string flag = cursor.nextToken();
    const auto found = byFlag.find(flag);
    if (found == byFlag.end()) {
      throw UnknownOptionException(flag);
    }
    const OptionSchema<Config>& option = *found->second;
    option.apply(config, detail::collectValues(option, cursor));
  }

  if (postValidate) {
    postValidate(config);
  }

  return config;
}

}  // namespace zappy::cli
