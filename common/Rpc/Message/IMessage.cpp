/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Abstract protocol message instance — opcode + typed argument access
*/

#include "Rpc/Message/IMessage.hpp"
#include <utility>

namespace zappy::rpc {

AMessage::AMessage(std::string opcode, std::vector<std::string> args)
    : opcode_(std::move(opcode)), args_(std::move(args)) {}

AMessage::AMessage(std::string opcode) : opcode_(std::move(opcode)) {}

const std::string &AMessage::opcode() const { return opcode_; }

const std::string &AMessage::arg(std::size_t index) const {
  return args_.at(index);
}

std::size_t AMessage::argCount() const { return args_.size(); }

const std::vector<std::string> &AMessage::args() const { return args_; }

ParsedMessage::ParsedMessage(std::string opcode, std::vector<std::string> args)
    : AMessage(std::move(opcode), std::move(args)) {}

}  // namespace zappy::rpc
