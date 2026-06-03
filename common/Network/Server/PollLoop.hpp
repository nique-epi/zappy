/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** poll() event loop — accept, read, write, with injectable wake-up timeout
*/

#pragma once

#include <poll.h>
#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "Network/Server/Client.hpp"
#include "Network/Socket/ISocket.hpp"

namespace zappy::network {

/**
 * @brief poll(2)-based server event loop: accepts connections, reads lines,
 *        writes replies.
 *
 * All I/O is done through ISocket so the loop is fully testable without
 * threads. The poll timeout is injectable so a game scheduler can wake the
 * loop at the next scheduled deadline instead of blocking indefinitely.
 */
class PollLoop {
 public:
  using CommandHandler = std::function<void(Client &, const std::string &)>;
  using ConnectHandler = std::function<void(int fd)>;
  using DisconnectHandler = std::function<void(int fd)>;

  PollLoop(int serverFd, ISocket &io);

  void setHandler(CommandHandler handler);
  void setConnectHandler(ConnectHandler handler);
  void setDisconnectHandler(DisconnectHandler handler);

  /**
   * @brief Run the loop until stop() is called.
   * @param timeoutMs Poll timeout in milliseconds; -1 blocks until activity.
   */
  void run(int timeoutMs = -1);

  /**
   * @brief Run a single poll iteration.
   * @param timeoutMs Poll timeout in milliseconds; -1 blocks until activity.
   */
  void runOnce(int timeoutMs = -1);

  void stop();

  void sendTo(int clientFd, const std::string &data);
  void broadcast(const std::string &data);

  std::size_t clientCount() const;

 private:
  void handleNewConnection();
  void handleClientRead(int fd);
  void handleClientWrite(int fd);
  void removeClient(int fd);
  void rebuildFds();

  int serverFd_;
  ISocket &io_;
  std::unordered_map<int, Client> clients_;
  std::vector<struct pollfd> fds_;
  CommandHandler handler_;
  ConnectHandler connectHandler_;
  DisconnectHandler disconnectHandler_;
  bool running_;
  bool dirty_;
};

}  // namespace zappy::network
