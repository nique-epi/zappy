/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** IFieldType — abstract field type interface
*/

#pragma once

#include <string>

namespace zappy::schema {

/**
 * @brief Contract for a positional schema field type.
 */
class IFieldType {
 public:
  virtual ~IFieldType() = default;

  /**
   * @brief Human-readable field name, surfaced in SchemaError.
   */
  virtual const std::string &name() const = 0;

  /**
   * @brief Validate a raw token value against this field type.
   */
  virtual bool validate(const std::string &value) const = 0;

  /**
   * @brief Short type label used in error messages.
   */
  virtual const char *typeName() const = 0;

  /**
   * @brief Whether this field, when last, absorbs every remaining token.
   *
   * A greedy trailing field lets unquoted free-form text (e.g. an AI
   * Broadcast message) be carried by a single field instead of failing the
   * fixed-arity check.
   */
  virtual bool consumesRemainder() const = 0;
};

}  // namespace zappy::schema
