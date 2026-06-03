/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** LongStringFieldType — possibly multi-word string field
*/

#pragma once

#include <string>
#include "Schema/Fields/StringFieldType.hpp"

namespace zappy::schema {

/**
 * @brief Field type for multi-word strings.
 *
 * Inherits non-empty validation from StringFieldType; only the type label
 * differs.
 */
class LongStringFieldType : public StringFieldType {
 public:
  explicit LongStringFieldType(std::string name);
  const char *typeName() const override;
  bool consumesRemainder() const override;
};

}  // namespace zappy::schema
