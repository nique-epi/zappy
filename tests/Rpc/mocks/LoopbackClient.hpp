/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Real loopback TCP client used to drive RPCServer integration tests
*/

#pragma once

#include <string>

namespace zappy::rpc::test {

/**
 * @brief RAII TCP client connected to 127.0.0.1 for exercising a live server.
 *
 * Used to drive the real RPCServer (which owns a RealSocket and cannot take a
 * mock) over the loopback interface. The receive timeout is bounded so a
 * missing reply fails the test instead of hanging it.
 */
class LoopbackClient {
 public:
  /**
   * @brief Connect to 127.0.0.1 on @p port with a bounded receive timeout.
   * @throws std::runtime_error if socket(2) or connect(2) fails.
   */
  explicit LoopbackClient(int port);
  ~LoopbackClient();

  LoopbackClient(const LoopbackClient &) = delete;
  LoopbackClient &operator=(const LoopbackClient &) = delete;
  LoopbackClient(LoopbackClient &&) = delete;
  LoopbackClient &operator=(LoopbackClient &&) = delete;

  /**
   * @brief Send a line, appending a trailing newline.
   */
  void sendLine(const std::string &line);

  /**
   * @brief Block until bytes arrive or the receive timeout elapses.
   * @returns The bytes received (empty on timeout or clean close).
   */
  std::string receive();

  int fd() const;

 private:
  int fd_;
};

}  // namespace zappy::rpc::test
