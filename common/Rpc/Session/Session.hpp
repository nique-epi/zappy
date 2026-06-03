/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Session<Ctx> — typed per-client handle with user-defined context
*/

#pragma once

#include <functional>
#include <string>
#include <utility>

namespace zappy::rpc {

/**
 * @brief Per-client handle passed to every RPC handler.
 *
 * Wraps a client fd and an opaque send callback so handlers can reply without
 * knowing the underlying transport. The Ctx slot is value-initialised on
 * connect and lives for the duration of the connection.
 */
template <typename Ctx>
class Session {
 public:
  using Sender = std::function<void(const std::string &)>;

  Session(int fd, Sender sender) : fd_(fd), sender_(std::move(sender)) {}

  /**
   * @brief File descriptor of this connection.
   */
  int fd() const { return fd_; }

  /**
   * @brief Enqueue a line to this client (newline appended automatically).
   */
  void send(const std::string &line) { sender_(line + "\n"); }

  /**
   * @brief Application-level context attached to this connection.
   */
  Ctx &ctx() { return ctx_; }
  const Ctx &ctx() const { return ctx_; }

  /**
   * @brief Whether the connection cleared the handshake and may dispatch.
   *
   * A server with no handshake handler treats every session as authenticated.
   */
  bool isAuthenticated() const { return authenticated_; }

  /**
   * @brief Mark the handshake as completed; subsequent lines are dispatched.
   */
  void completeHandshake() { authenticated_ = true; }

 private:
  int fd_;
  Sender sender_;
  Ctx ctx_{};
  bool authenticated_{false};
};

}  // namespace zappy::rpc
