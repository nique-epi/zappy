/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Command-line parser exception hierarchy
*/

#include "Cli/Exceptions/ParserException.hpp"

namespace zappy::server {

ParserException::ParserException(const std::string &message)
    : std::runtime_error(message) {}

UnknownOptionException::UnknownOptionException(const std::string &option)
    : ParserException("unknown option " + option) {}

MissingValueException::MissingValueException(const std::string &option)
    : ParserException("missing value for " + option) {}

InvalidValueException::InvalidValueException(const std::string &option,
                                             const std::string &value,
                                             const std::string &reason)
    : ParserException("invalid value '" + value + "' for " + option + ": " +
                      reason) {}

}  // namespace zappy::server
