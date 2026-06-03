/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WireFormat — builds a protocol line from an opcode and argument values
*/

#include "Rpc/Message/WireFormat.hpp"
#include <stdexcept>

namespace zappy::rpc {

namespace {
bool containsLineBreak(const std::string &value) {
  return value.find('\n') != std::string::npos ||
         value.find('\r') != std::string::npos;
}
}  // namespace

std::string buildWireLine(const std::string &opcode,
                          const std::vector<std::string> &args) {
  if (containsLineBreak(opcode)) {
    throw std::invalid_argument("buildWireLine: opcode contains CR/LF");
  }
  std::string line = opcode;
  for (const auto &value : args) {
    if (containsLineBreak(value)) {
      throw std::invalid_argument(
          "buildWireLine: argument value contains CR/LF");
    }
    line += ' ';
    bool needsQuote = value.empty() || value.find(' ') != std::string::npos ||
                      value.find('"') != std::string::npos;
    if (needsQuote) {
      line += '"';
      for (char character : value) {
        if (character == '"') {
          line += '\\';
        }
        line += character;
      }
      line += '"';
    } else {
      line += value;
    }
  }
  return line;
}

}  // namespace zappy::rpc
