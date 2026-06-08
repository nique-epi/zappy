/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** INetworkClient — interface for a writable/listenable server connection
*/

#pragma once

#include <functional>
#include <string>

namespace zappy::gui {

/**
 * @brief Minimal interface for a line-oriented TCP connection to the server.
 *
 * Implemented by NetworkManager for production and by test doubles during
 * unit testing. ServerHandshake and future command processors depend on this
 * interface rather than the concrete NetworkManager class.
 */
class INetworkClient {
 public:
  using ResponseHandler = std::function<void(const std::string&)>;

  INetworkClient() = default;
  virtual ~INetworkClient() = default;
  INetworkClient(const INetworkClient&) = delete;
  INetworkClient& operator=(const INetworkClient&) = delete;
  INetworkClient(INetworkClient&&) = delete;
  INetworkClient& operator=(INetworkClient&&) = delete;

  /**
   * @brief Register the handler called for each complete line from the server.
   */
  virtual void setResponseHandler(ResponseHandler handler) = 0;

  /**
   * @brief Send a line to the server (newline appended automatically).
   */
  virtual void sendLine(const std::string& line) = 0;
};

}  // namespace zappy::gui
