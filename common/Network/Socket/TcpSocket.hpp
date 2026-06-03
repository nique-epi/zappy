/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RAII TCP socket wrapper (shared between server and client)
*/

#pragma once

namespace zappy::network {

/**
 * @brief RAII owner of a TCP socket file descriptor.
 *
 * Non-copyable; movable. Closes the fd in the destructor.
 */
class TcpSocket {
 public:
  /**
   * @brief Create a new AF_INET/SOCK_STREAM socket.
   * @throws SocketError if socket(2) fails.
   */
  TcpSocket();
  ~TcpSocket();

  TcpSocket(const TcpSocket &) = delete;
  TcpSocket &operator=(const TcpSocket &) = delete;
  TcpSocket(TcpSocket &&other) noexcept;
  TcpSocket &operator=(TcpSocket &&other) noexcept;

  /**
   * @brief Enable SO_REUSEADDR on the socket.
   * @throws SocketError if setsockopt(2) fails.
   */
  void setReuseAddress();

  /**
   * @brief Return the underlying file descriptor.
   * @returns The socket fd, or -1 if moved-from.
   */
  int fd() const;

 private:
  int fd_;
};

}  // namespace zappy::network
