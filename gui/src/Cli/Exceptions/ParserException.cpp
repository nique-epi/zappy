/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Command-line parser exception hierarchy
*/

#include "Cli/Exceptions/ParserException.hpp"
#include "Cli/ParserMessages.hpp"

namespace zappy::gui {

namespace msg = zappy::gui::messages;

ParserException::ParserException(const std::string& message)
    : std::runtime_error(message) {}

UnknownOptionException::UnknownOptionException(const std::string& option)
    : ParserException(msg::UNKNOWN_OPTION + option) {}

MissingValueException::MissingValueException(const std::string& option)
    : ParserException(msg::MISSING_VALUE + option) {}

InvalidValueException::InvalidValueException(const std::string& option,
                                             const std::string& value,
                                             const std::string& reason)
    : ParserException(msg::INVALID_VALUE_PREFIX + value +
                      msg::INVALID_VALUE_INFIX + option +
                      msg::REASON_SEPARATOR + reason) {}

}  // namespace zappy::gui
