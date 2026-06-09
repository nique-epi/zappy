/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** NetworkManager — non-blocking TCP connection to the Zappy server
*/

#pragma once

#include <memory>
#include <string>
#include "Network/Client/ClientLoop.hpp"
#include "Network/INetworkClient.hpp"
#include "Network/Socket/RealSocket.hpp"

namespace zappy::gui {

/**
 * @brief RAII owner of a non-blocking TCP connection to the Zappy server.
 *
 * Resolves @p hostname via getaddrinfo(3), connects with a timeout using
 * non-blocking connect(2) + poll(2), and leaves the socket in O_NONBLOCK
 * mode so recv(2) calls in the Raylib render loop never block.
 *
 * Internally wraps a ClientLoop with stdinFd = -1 (GUI drives its own
 * input). Call @ref runOnce each frame to drain any pending server data.
 */
class NetworkManager : public INetworkClient {
 public:
  using ResponseHandler = INetworkClient::ResponseHandler;

  /**
   * @brief Resolve @p hostname and connect to @p port with a timeout.
   *
   * @param hostname Server hostname or dotted-decimal address.
   * @param port     TCP port.
   * @throws zappy::network::ConnectError on resolution failure.
   * @throws zappy::network::ConnectError on connection timeout or refusal.
   */
  NetworkManager(const std::string& hostname, int port);
  ~NetworkManager();

  NetworkManager(const NetworkManager&) = delete;
  NetworkManager& operator=(const NetworkManager&) = delete;
  NetworkManager(NetworkManager&&) = delete;
  NetworkManager& operator=(NetworkManager&&) = delete;

  /**
   * @brief Register the handler called for each line received from the server.
   */
  void setResponseHandler(ResponseHandler handler) override;

  /**
   * @brief Poll the server socket once without blocking.
   *
   * Call once per Raylib frame. Drains all available data and dispatches
   * lines to the registered @ref ResponseHandler.
   *
   * @param timeoutMs Poll timeout in milliseconds; 0 returns immediately.
   */
  void runOnce(int timeoutMs = 0);

  /**
   * @brief Send a raw line to the server (newline appended automatically).
   */
  void sendLine(const std::string& line) override;

  /**
   * @brief Signal the loop to stop on the next @ref runOnce call.
   */
  void stop();

 private:
  int fd_ = -1;
  zappy::network::RealSocket io_;
  std::unique_ptr<zappy::network::ClientLoop> loop_ = nullptr;
};

}  // namespace zappy::gui
