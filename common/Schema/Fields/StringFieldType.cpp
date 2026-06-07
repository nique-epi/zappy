/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** StringFieldType — non-empty string field
*/

#include "Schema/Fields/StringFieldType.hpp"
#include <utility>
#include "Error/Messages/SchemaMessages.hpp"

namespace zappy::schema {

StringFieldType::StringFieldType(std::string name) : name_(std::move(name)) {}

const std::string &StringFieldType::name() const { return name_; }

const char *StringFieldType::typeName() const {
  return error::messages::STRING_TYPE;
}

bool StringFieldType::validate(const std::string &value) const {
  return !value.empty();
}

bool StringFieldType::consumesRemainder() const { return false; }

}  // namespace zappy::schema
