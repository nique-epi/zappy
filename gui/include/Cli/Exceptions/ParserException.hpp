/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Command-line parser exception hierarchy
*/

#pragma once

#include <stdexcept>
#include <string>

namespace zappy::gui {

/**
 * @brief Root exception for every command-line parsing error.
 *
 * The argument parser never lets a standard-library exception escape: any
 * malformed, missing, unknown, or out-of-range option surfaces as a subclass
 * of @ref ParserException so callers can catch the whole family at once.
 */
class ParserException : public std::runtime_error {
 public:
  explicit ParserException(const std::string& message);
};

/**
 * @brief Thrown when an argument is not a recognised option flag.
 */
class UnknownOptionException : public ParserException {
 public:
  explicit UnknownOptionException(const std::string& option);
};

/**
 * @brief Thrown when an option that requires a value is missing one, or when
 *        a mandatory flag was not supplied at all.
 */
class MissingValueException : public ParserException {
 public:
  explicit MissingValueException(const std::string& option);
};

/**
 * @brief Thrown when an option value is not a valid integer or is out of range.
 */
class InvalidValueException : public ParserException {
 public:
  InvalidValueException(const std::string& option, const std::string& value,
                        const std::string& reason);
};

}  // namespace zappy::gui
