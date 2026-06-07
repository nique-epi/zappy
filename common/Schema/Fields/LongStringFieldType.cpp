/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** LongStringFieldType — possibly multi-word string field
*/

#include "Schema/Fields/LongStringFieldType.hpp"
#include <utility>
#include "Error/Messages/SchemaMessages.hpp"

namespace zappy::schema {

LongStringFieldType::LongStringFieldType(std::string name)
    : StringFieldType(std::move(name)) {}

const char *LongStringFieldType::typeName() const {
  return error::messages::LONG_STRING_TYPE;
}

bool LongStringFieldType::consumesRemainder() const { return true; }

}  // namespace zappy::schema
