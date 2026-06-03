/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RAII TCP connection to a remote server (client side)
*/

#pragma once

#include <string>

namespace zappy::network {

/**
 * @brief RAII owner of a connected TCP socket to a remote server.
 *
 * Non-copyable; movable. Closes the fd in the destructor. Disables Nagle's
 * algorithm (TCP_NODELAY) so small protocol lines are sent without delay.
 */
class Connection {
 public:
  /**
   * @brief Connect to @p ipAddress : @p port with a blocking connect(2).
   * @param ipAddress Dotted-decimal IPv4 address of the server.
   * @param port TCP port to connect to.
   * @throws ConnectError if socket(2), the address, or connect(2) fails.
   */
  Connection(const std::string &ipAddress, int port);
  ~Connection();

  Connection(const Connection &) = delete;
  Connection &operator=(const Connection &) = delete;
  Connection(Connection &&other) noexcept;
  Connection &operator=(Connection &&other) noexcept;

  /**
   * @brief Return the underlying connected file descriptor.
   * @returns The socket fd, or -1 if moved-from.
   */
  int fd() const;

 private:
  int fd_;
};

}  // namespace zappy::network
