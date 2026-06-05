/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** OptionSchema — declarative description of every server command-line option
*/

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "App/ServerConfig.hpp"
#include "Schema/Fields/IFieldType.hpp"

namespace zappy::server {

/**
 * @brief Whether an option takes exactly one value or a list of values.
 */
enum class OptionArity : std::uint8_t { SingleValue, ValueList };

/**
 * @brief Writes an option's already-validated value(s) into the configuration.
 */
using OptionApplier = std::function<void(
    ServerConfig &config, const std::vector<std::string> &values)>;

/**
 * @brief Declarative description of a single command-line option.
 *
 * Validation is delegated to a @ref zappy::schema::IFieldType so the
 * command-line parser reuses the same field-type vocabulary as the RPC schema.
 * The parser validates every value against @ref fieldType before invoking
 * @ref apply, which only assigns.
 */
struct OptionSchema {
  std::string flag;
  std::string description;
  OptionArity arity;
  std::shared_ptr<zappy::schema::IFieldType> fieldType;
  OptionApplier apply;
};

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
