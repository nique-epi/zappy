/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** NumberFieldType — decimal integer field (optionally negative)
*/

#include "Schema/Fields/NumberFieldType.hpp"
#include <cstddef>
#include <utility>
#include "Error/Messages/SchemaMessages.hpp"

namespace zappy::schema {

NumberFieldType::NumberFieldType(std::string name) : name_(std::move(name)) {}

const std::string &NumberFieldType::name() const { return name_; }

const char *NumberFieldType::typeName() const {
  return error::messages::NUMBER_TYPE;
}

bool NumberFieldType::validate(const std::string &value) const {
  if (value.empty()) {
    return false;
  }
  std::size_t start = (value[0] == '-') ? 1 : 0;
  if (start == value.size()) {
    return false;
  }
  for (std::size_t i = start; i < value.size(); ++i) {
    if (value[i] < '0' || value[i] > '9') {
      return false;
    }
  }
  return true;
}

bool NumberFieldType::consumesRemainder() const { return false; }

}  // namespace zappy::schema
