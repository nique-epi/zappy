/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SchemaError — field name plus human-readable validation message
*/

#include "Schema/SchemaError.hpp"

namespace zappy::schema {

std::string SchemaError::what() const { return field + ": " + message; }

}  // namespace zappy::schema
