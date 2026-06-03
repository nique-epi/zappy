/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResponseDispatcher — routes server→client lines to registered handlers
*/

#include "Rpc/Client/ResponseDispatcher.hpp"
#include <utility>
#include "Rpc/Message/MessageParser.hpp"

namespace zappy::rpc {

ResponseDispatcher &ResponseDispatcher::on(const RPCMessage &message,
                                           Handler handler) {
  handlers_.emplace(message.opcode(), std::move(handler));
  return *this;
}

ResponseDispatcher &ResponseDispatcher::on(const std::string &opcode,
                                           Handler handler) {
  handlers_.emplace(opcode, std::move(handler));
  return *this;
}

ResponseDispatcher &ResponseDispatcher::onUnknown(Handler handler) {
  unknownHandler_ = std::move(handler);
  return *this;
}

void ResponseDispatcher::dispatch(IMessage &message) {
  auto it = handlers_.find(message.opcode());
  if (it != handlers_.end()) {
    it->second(message);
  } else if (unknownHandler_) {
    unknownHandler_(message);
  }
}

void ResponseDispatcher::operator()(const std::string &line) {
  ParsedMessage message = parseMessage(line);
  if (message.opcode().empty()) {
    return;
  }
  dispatch(message);
}

}  // namespace zappy::rpc
