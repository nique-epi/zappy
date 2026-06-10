/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MessageParser — dispatch incoming server lines to WorldState updates
*/

#pragma once

#include <string>
#include "Network/Parsing/HandlerTypes.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Parses every line-oriented message from the Zappy server and
 *        applies the corresponding update to a @ref WorldState.
 *
 * Installed as the @ref INetworkClient response handler once the handshake
 * completes. Each recognised command token maps to a stored lambda; unknown
 * tokens are silently ignored.
 */
class MessageParser {
 public:
  explicit MessageParser(WorldState& world);
  ~MessageParser() = default;

  MessageParser(const MessageParser&) = delete;
  MessageParser& operator=(const MessageParser&) = delete;
  MessageParser(MessageParser&&) = delete;
  MessageParser& operator=(MessageParser&&) = delete;

  /**
   * @brief Parse one server line and apply the matching WorldState update.
   *
   * @param line Complete line received from the server (no trailing newline).
   */
  void parseLine(const std::string& line);

 private:
  WorldState& world_;
  HandlerMap handlers_;
};

}  // namespace zappy::gui
