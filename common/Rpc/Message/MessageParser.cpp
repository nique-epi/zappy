/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Protocol line parser — produces a ParsedMessage from a raw line
*/

#include "Rpc/Message/MessageParser.hpp"
#include <iterator>
#include <utility>
#include <vector>
#include "Schema/Tokenizer.hpp"

namespace zappy::rpc {

ParsedMessage parseMessage(const std::string &line) {
  auto tokens = schema::tokenize(line);
  if (tokens.empty()) {
    return ParsedMessage("", {});
  }
  std::string opcode = std::move(tokens[0]);
  std::vector<std::string> args(std::make_move_iterator(tokens.begin() + 1),
                                std::make_move_iterator(tokens.end()));
  return ParsedMessage(std::move(opcode), std::move(args));
}

}  // namespace zappy::rpc
