/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SchemaError — field name plus human-readable validation message
*/

#pragma once

#include <string>

namespace zappy::schema {

/**
 * @brief Holds the field name and message produced when parsing fails.
 */
struct SchemaError {
  std::string field;
  std::string message;

  /**
   * @brief Render the error as "field: message".
   */
  std::string what() const;
};

}  // namespace zappy::schema
