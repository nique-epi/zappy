/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WireFormat — builds a protocol line from an opcode and argument values
*/

#include "Rpc/Message/WireFormat.hpp"

namespace zappy::rpc {

std::string buildWireLine(const std::string &opcode,
                          const std::vector<std::string> &args) {
  std::string line = opcode;
  for (const auto &value : args) {
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
