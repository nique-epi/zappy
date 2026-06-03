/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RPCServer<Ctx> — TCP server facade over PollLoop + Dispatcher
*/

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Network/Exceptions.hpp"
#include "Network/Server/PollLoop.hpp"
#include "Network/Socket/RealSocket.hpp"
#include "Network/Socket/TcpSocket.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Message/RPCMessage.hpp"
#include "Rpc/Message/TypedMessage.hpp"
#include "Rpc/Server/Dispatcher.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::rpc {

/**
 * @brief Top-level RPC server: bind/listen, poll loop, typed message dispatch.
 *
 * Ctx is the application-level context attached to every connection. It is
 * value-initialised on connect and accessible via session.ctx(). The poll
 * timeout is injectable through run()/runOnce() so a game scheduler can drive
 * wake-ups.
 */
template <typename Ctx>
class RPCServer {
 public:
  using Handler = std::function<void(Session<Ctx> &, IMessage &)>;

  explicit RPCServer(int port) : port_(port), loop_(socket_.fd(), io_) {
    loop_.setHandler([this](network::Client &client, const std::string &line) {
      auto it = sessions_.find(client.fd());
      if (it == sessions_.end()) {
        return;
      }
      Session<Ctx> &session = it->second;
      if (handshakeHandler_ && !session.isAuthenticated()) {
        handshakeHandler_(session, line);
        return;
      }
      dispatcher_(session, line);
    });
    loop_.setConnectHandler([this](int fd) {
      auto [it, inserted] = sessions_.emplace(
          fd, Session<Ctx>{fd, [this, fd](const std::string &line) {
                             loop_.sendTo(fd, line);
                           }});
      if (connectHandler_) {
        connectHandler_(it->second);
      }
    });
    loop_.setDisconnectHandler([this](int fd) {
      auto it = sessions_.find(fd);
      if (it != sessions_.end()) {
        if (disconnectHandler_) {
          disconnectHandler_(it->second);
        }
        sessions_.erase(it);
      }
    });
  }

  /**
   * @brief Register a typed handler bound to a TypedMessage<T>.
   */
  template <typename T, typename F>
  RPCServer &on(const TypedMessage<T> &message, F &&handler) {
    dispatcher_.on(message, std::forward<F>(handler));
    return *this;
  }

  /**
   * @brief Register a raw handler for an RPCMessage (routes by opcode).
   */
  RPCServer &on(const RPCMessage &message, Handler handler) {
    dispatcher_.on(message.opcode(), std::move(handler));
    return *this;
  }

  /**
   * @brief Register a raw handler for an opcode string (no validation).
   */
  RPCServer &on(const std::string &opcode, Handler handler) {
    dispatcher_.on(opcode, std::move(handler));
    return *this;
  }

  RPCServer &onUnknown(Handler handler) {
    dispatcher_.onUnknown(std::move(handler));
    return *this;
  }

  /**
   * @brief Register the schema-validation-failure handler (see
   *        Dispatcher::onInvalid).
   */
  RPCServer &onInvalid(Handler handler) {
    dispatcher_.onInvalid(std::move(handler));
    return *this;
  }

  /**
   * @brief Register the per-connection handshake handler.
   *
   * While set, the first lines of a connection are routed here instead of the
   * opcode dispatcher until the handler calls session.completeHandshake().
   * This is where a protocol resolves its greeting (e.g. the Zappy team-name
   * exchange that distinguishes a GUI from an AI client).
   */
  RPCServer &onHandshake(
      std::function<void(Session<Ctx> &, const std::string &)> handler) {
    handshakeHandler_ = std::move(handler);
    return *this;
  }

  RPCServer &onConnect(std::function<void(Session<Ctx> &)> handler) {
    connectHandler_ = std::move(handler);
    return *this;
  }

  RPCServer &onDisconnect(std::function<void(Session<Ctx> &)> handler) {
    disconnectHandler_ = std::move(handler);
    return *this;
  }

  /**
   * @brief Send a message to a specific session.
   */
  template <typename M>
  void respond(Session<Ctx> &session, const M &message,
               const std::vector<std::string> &args = {}) {
    session.send(message.buildLine(args));
  }

  void respond(Session<Ctx> &session, const std::string &line) {
    session.send(line);
  }

  /**
   * @brief Push an unsolicited event to a specific session.
   */
  template <typename M>
  void emit(Session<Ctx> &session, const M &message,
            const std::vector<std::string> &args = {}) {
    session.send(message.buildLine(args));
  }

  /**
   * @brief Broadcast a message to every connected client.
   */
  template <typename M>
  void broadcast(const M &message, const std::vector<std::string> &args = {}) {
    loop_.broadcast(message.buildLine(args) + "\n");
  }

  void broadcast(const std::string &line) { loop_.broadcast(line + "\n"); }

  /**
   * @brief Iterate over all connected sessions.
   */
  template <typename Fn>
  void forEachSession(Fn &&fn) {
    for (auto &[fd, session] : sessions_) {
      fn(session);
    }
  }

  void start() {
    socket_.setReuseAddress();
    bindAndListen();
  }

  /**
   * @brief Return the actual port the server is listening on.
   */
  int port() const { return port_; }

  void run(int timeoutMs = -1) { loop_.run(timeoutMs); }
  void runOnce(int timeoutMs = -1) { loop_.runOnce(timeoutMs); }
  void stop() { loop_.stop(); }

 private:
  void bindAndListen() {
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(static_cast<uint16_t>(port_));

    if (bind(socket_.fd(), reinterpret_cast<struct sockaddr *>(&address),
             sizeof(address)) < 0) {
      throw network::BindError("bind() failed");
    }
    if (port_ == 0) {
      struct sockaddr_in bound;
      socklen_t length = sizeof(bound);
      getsockname(socket_.fd(), reinterpret_cast<struct sockaddr *>(&bound),
                  &length);
      port_ = ntohs(bound.sin_port);
    }
    if (listen(socket_.fd(), SOMAXCONN) < 0) {
      throw network::ListenError("listen() failed");
    }
  }

  int port_;
  network::TcpSocket socket_;
  network::RealSocket io_;
  network::PollLoop loop_;
  Dispatcher<Ctx> dispatcher_;
  std::unordered_map<int, Session<Ctx>> sessions_;
  std::function<void(Session<Ctx> &)> connectHandler_;
  std::function<void(Session<Ctx> &)> disconnectHandler_;
  std::function<void(Session<Ctx> &, const std::string &)> handshakeHandler_;
};

}  // namespace zappy::rpc
