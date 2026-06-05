/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgumentCursor — forward cursor over argv with value extraction
*/

#include "Cli/ArgumentCursor.hpp"
#include "Cli/Exceptions/ParserException.hpp"

namespace zappy::server {

namespace {

bool isOption(const char *token) { return token[0] == '-' && token[1] != '\0'; }

}  // namespace

ArgumentCursor::ArgumentCursor(int argumentCount, char **arguments)
    : argumentCount_(argumentCount), arguments_(arguments), index_(0) {}

bool ArgumentCursor::hasNextToken() const {
  return index_ + 1 < argumentCount_;
}

std::string ArgumentCursor::nextToken() {
  index_ += 1;
  return arguments_[index_];
}

bool ArgumentCursor::nextIsValue() const {
  return hasNextToken() && !isOption(arguments_[index_ + 1]);
}

std::string ArgumentCursor::requireValue(const std::string &option) {
  if (!nextIsValue()) {
    throw MissingValueException(option);
  }
  return nextToken();
}

}  // namespace zappy::server
