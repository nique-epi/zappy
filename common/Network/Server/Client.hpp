/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Connected client state — pure buffer, no I/O syscalls
*/

#pragma once

#include <cstddef>
#include <string>

namespace zappy::network {

/**
 * @brief Connected client state — owns the fd and two line-oriented buffers.
 *
 * No I/O syscalls here; PollLoop performs all reads and writes.
 */
class Client {
 public:
  explicit Client(int fd);
  ~Client();

  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;
  Client(Client &&other) noexcept;
  Client &operator=(Client &&other) noexcept;

  int fd() const;

  void appendInput(const char *data, std::size_t length);

  /**
   * @brief Consume one newline-terminated line from the inbound buffer.
   * @param out Receives the line without its trailing newline.
   * @returns true if a complete line was consumed, false if none is ready.
   */
  bool consumeLine(std::string &out);

  void appendOutput(const std::string &data);
  bool hasPendingOutput() const;
  const std::string &peekOutput() const;
  void consumeOutput(std::size_t count);

 private:
  int fd_;
  std::string inputBuffer_;
  std::string outputBuffer_;
};

}  // namespace zappy::network
