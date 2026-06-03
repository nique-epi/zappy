/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** LongStringFieldType — possibly multi-word string field
*/

#include "Schema/Fields/LongStringFieldType.hpp"
#include <utility>

namespace zappy::schema {

LongStringFieldType::LongStringFieldType(std::string name)
    : StringFieldType(std::move(name)) {}

const char *LongStringFieldType::typeName() const { return "long_string"; }

bool LongStringFieldType::consumesRemainder() const { return true; }

}  // namespace zappy::schema
