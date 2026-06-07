/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SchemaMessages — user-facing strings emitted by the schema validation layer
*/

#pragma once

namespace zappy::error::messages {

inline constexpr const char *NUMBER_TYPE = "number";
inline constexpr const char *STRING_TYPE = "string";
inline constexpr const char *LONG_STRING_TYPE = "long_string";

inline constexpr const char *BOUNDED_NUMBER_PREFIX = "number in [";
inline constexpr const char *BOUNDED_NUMBER_SEPARATOR = ", ";
inline constexpr const char *BOUNDED_NUMBER_SUFFIX = "]";

inline constexpr const char *INVERTED_BOUNDS_PREFIX =
    "BoundedNumberFieldType: minimum (";
inline constexpr const char *INVERTED_BOUNDS_INFIX =
    ") must not exceed maximum (";
inline constexpr const char *INVERTED_BOUNDS_SUFFIX = ")";

}  // namespace zappy::error::messages
