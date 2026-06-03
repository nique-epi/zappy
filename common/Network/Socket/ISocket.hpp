/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Abstract I/O backend — injectable for testing
*/

#pragma once

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace zappy::network {

/**
 * @brief Abstract I/O backend.
 *
 * Inject a mock implementation to test networking code without real sockets.
 */
class ISocket {
 public:
  virtual ~ISocket() = default;

  /**
   * @brief Wait for I/O events on a set of file descriptors.
   * @param fds Array of pollfd structures to monitor.
   * @param nfds Number of entries in @p fds.
   * @param timeout Maximum wait in milliseconds; -1 to block indefinitely.
   * @returns Number of fds with events, 0 on timeout, -1 on error.
   */
  virtual int doPoll(struct pollfd *fds, nfds_t nfds, int timeout) = 0;

  /**
   * @brief Accept an incoming connection on a listening socket.
   * @param fd Listening socket file descriptor.
   * @param address Output peer address (may be nullptr).
   * @param length In/out size of @p address buffer.
   * @returns New connected fd, or -1 on error.
   */
  virtual int doAccept(int fd, struct sockaddr *address, socklen_t *length) = 0;

  /**
   * @brief Receive data from a connected socket.
   * @param fd Socket file descriptor.
   * @param buffer Destination buffer.
   * @param length Buffer capacity in bytes.
   * @param flags recv(2) flags.
   * @returns Bytes read, 0 on clean disconnect, -1 on error.
   */
  virtual ssize_t doReceive(int fd, void *buffer, size_t length, int flags) = 0;

  /**
   * @brief Send data on a connected socket.
   * @param fd Socket file descriptor.
   * @param buffer Source buffer.
   * @param length Number of bytes to send.
   * @param flags send(2) flags.
   * @returns Bytes written, or -1 on error.
   */
  virtual ssize_t doSend(int fd, const void *buffer, size_t length,
                         int flags) = 0;
};

}  // namespace zappy::network
