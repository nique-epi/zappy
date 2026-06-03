/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RPCMessage — shared protocol message definition (opcode + wire builder)
*/

#include "Rpc/Message/RPCMessage.hpp"
#include <utility>
#include "Rpc/Message/WireFormat.hpp"

namespace zappy::rpc {

RPCMessage::RPCMessage(std::string opcode) : opcode_(std::move(opcode)) {}

const std::string &RPCMessage::opcode() const { return opcode_; }

std::string RPCMessage::buildLine(const std::vector<std::string> &args) const {
  return buildWireLine(opcode_, args);
}

}  // namespace zappy::rpc
