/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Command-line parser exception hierarchy
*/

#include "Cli/Exceptions/ParserException.hpp"
#include "Error/Messages/ParserMessages.hpp"

namespace zappy::server {

ParserException::ParserException(const std::string &message)
    : std::runtime_error(message) {}

UnknownOptionException::UnknownOptionException(const std::string &option)
    : ParserException(error::messages::UNKNOWN_OPTION + option) {}

MissingValueException::MissingValueException(const std::string &option)
    : ParserException(error::messages::MISSING_VALUE + option) {}

InvalidValueException::InvalidValueException(const std::string &option,
                                             const std::string &value,
                                             const std::string &reason)
    : ParserException(error::messages::INVALID_VALUE_PREFIX + value +
                      error::messages::INVALID_VALUE_INFIX + option +
                      error::messages::REASON_SEPARATOR + reason) {}

}  // namespace zappy::server
