/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** OptionSchema — generic declarative option description + schema-driven parser
*/

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "Schema/Fields/IFieldType.hpp"

namespace zappy::cli {

/**
 * @brief Whether an option takes exactly one value or a list of values.
 */
enum class OptionArity : std::uint8_t { SingleValue, ValueList };

/**
 * @brief Declarative description of a single command-line option.
 *
 * @tparam Config The configuration struct populated by @ref apply.
 */
template <typename Config>
struct OptionSchema {
  std::string flag;
  std::string description;
  OptionArity arity;
  std::shared_ptr<zappy::schema::IFieldType> fieldType;
  std::function<void(Config&, const std::vector<std::string>&)> apply;
};

/**
 * @brief Parse @p arguments against @p schema, populate a @p Config, then
 *        optionally run @p postValidate.
 *
 * @tparam Config   The configuration struct to build.
 * @param argumentCount Number of entries in @p arguments (argc).
 * @param arguments     Argument vector (argv).
 * @param schema        Ordered list of recognised option descriptions.
 * @param postValidate  Optional callback for cross-field validation; may
 *                      throw any @ref ParserException subclass.
 * @returns The populated configuration.
 * @throws UnknownOptionException On an unrecognised flag.
 * @throws MissingValueException  When a required value is absent.
 * @throws InvalidValueException  When a value fails its field-type check.
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
template <typename Config>
Config parseWithSchema(int argumentCount, char** arguments,
                       const std::vector<OptionSchema<Config>>& schema,
                       std::function<void(const Config&)> postValidate = {});

}  // namespace zappy::cli

#include "Cli/OptionSchema.tpp"
