/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RPCClient — TCP client facade over Connection + ClientLoop
*/

#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>
#include "Network/Client/ClientLoop.hpp"
#include "Network/Client/Connection.hpp"
#include "Network/Socket/ISocket.hpp"
#include "Network/Socket/RealSocket.hpp"
#include "Rpc/Client/ResponseDispatcher.hpp"
#include "Rpc/Message/RPCMessage.hpp"
#include "Rpc/Message/TypedMessage.hpp"

namespace zappy::rpc {

/**
 * @brief Top-level RPC client: connect, poll loop, typed message calls.
 *
 * Owns a Connection (when constructed from an address) and a ClientLoop that
 * multiplexes stdin and the server socket. Server lines are routed through a
 * ResponseDispatcher. The poll timeout is injectable through runOnce() so a
 * GUI render loop can drive its own cadence.
 */
class RPCClient {
 public:
  /**
   * @brief Connect to @p ipAddress : @p port, multiplexing stdin and socket.
   * @throws network::ConnectError if the connection fails.
   */
  RPCClient(const std::string &ipAddress, int port);

  /**
   * @brief Connect without polling stdin (e.g. a GUI driving its own input).
   * @throws network::ConnectError if the connection fails.
   */
  RPCClient(const std::string &ipAddress, int port, bool pollStdin);

  /**
   * @brief Inject a pre-connected fd and ISocket backend (e.g. a mock).
   * @param stdinFd fd to poll for stdin input; negative to disable.
   * @param serverFd Pre-connected server fd, owned by the caller.
   * @param io I/O backend, owned by the caller.
   */
  RPCClient(int stdinFd, int serverFd, network::ISocket &io);

  /**
   * @brief Register a typed handler bound to a TypedMessage<T>.
   */
  template <typename T, typename F>
  RPCClient &on(const TypedMessage<T> &message, F &&handler) {
    dispatcher_.on(message, std::forward<F>(handler));
    return *this;
  }

  /**
   * @brief Register a raw handler for an RPCMessage (routes by opcode).
   */
  RPCClient &on(const RPCMessage &message, ResponseDispatcher::Handler handler);

  /**
   * @brief Register a raw handler for an opcode string (no validation).
   */
  RPCClient &on(const std::string &opcode, ResponseDispatcher::Handler handler);

  /**
   * @brief Register a fallback handler for unrecognised server messages.
   */
  RPCClient &onUnknown(ResponseDispatcher::Handler handler);

  /**
   * @brief Register a handler for lines typed on stdin.
   */
  RPCClient &onStdin(std::function<void(const std::string &)> handler);

  /**
   * @brief Build and send a message from typed arguments.
   */
  template <typename M>
  void call(const M &message, const std::vector<std::string> &args) {
    loop_.sendLine(message.buildLine(args));
  }

  /**
   * @brief Enqueue a raw line to be sent to the server (newline appended).
   */
  void send(const std::string &line);

  void run();
  void runOnce(int timeoutMs = 50);
  void stop();

 private:
  std::optional<network::Connection> connection_;
  network::RealSocket realIo_;
  network::ClientLoop loop_;
  ResponseDispatcher dispatcher_;
};

}  // namespace zappy::rpc
