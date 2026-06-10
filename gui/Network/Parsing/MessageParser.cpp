/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MessageParser — dispatch incoming server lines to WorldState updates
*/

#include "Network/Parsing/MessageParser.hpp"
#include <sstream>
#include <string>
#include "Network/Parsing/Handlers.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
MessageParser::MessageParser(WorldState& world) : world_(world) {
  registerMapHandlers(handlers_, world_);
  registerPlayerHandlers(handlers_, world_);
  registerEggHandlers(handlers_, world_);
  registerServerHandlers(handlers_, world_);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void MessageParser::parseLine(const std::string& line) {
  std::istringstream stream(line);
  std::string command;
  stream >> command;
  if (const auto handler = handlers_.find(command);
      handler != handlers_.end()) {
    handler->second(stream);
  }
}

}  // namespace zappy::gui
