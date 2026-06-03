/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Client poll() event loop — multiplexes stdin and the server socket
*/

#pragma once

#include <functional>
#include <string>
#include "Network/Socket/ISocket.hpp"

namespace zappy::network {

/**
 * @brief poll(2)-based client loop multiplexing stdin and the server socket.
 *
 * All I/O goes through ISocket so the loop is fully testable without real
 * sockets or threads. Pass a negative stdin fd to poll the server socket only
 * (e.g. a GUI that drives its own input instead of reading lines from stdin).
 * The poll timeout is injectable so a render loop can drive its own cadence.
 */
class ClientLoop {
 public:
  using CommandHandler = std::function<void(const std::string &)>;
  using ResponseHandler = std::function<void(const std::string &)>;

  ClientLoop(int stdinFd, int serverFd, ISocket &io);

  void setCommandHandler(CommandHandler handler);
  void setResponseHandler(ResponseHandler handler);

  /**
   * @brief Enqueue a line to be sent to the server (newline appended).
   */
  void sendLine(const std::string &line);

  /**
   * @brief Run the loop until stop() is called or a peer disconnects.
   */
  void run();

  /**
   * @brief Run a single poll iteration.
   * @param timeoutMs Poll timeout in milliseconds; -1 blocks until activity.
   */
  void runOnce(int timeoutMs = -1);

  void stop();

 private:
  void handleStdin();
  void handleServerRead();
  void flushOutput();

  int stdinFd_;
  int serverFd_;
  ISocket &io_;
  std::string stdinBuffer_;
  std::string serverBuffer_;
  std::string outputBuffer_;
  CommandHandler commandHandler_;
  ResponseHandler responseHandler_;
  bool running_;
};

}  // namespace zappy::network
