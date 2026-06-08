/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GameServer — single-port server hosting both GUI and AI protocols
*/

#pragma once

#include "App/Scheduler/Scheduler.hpp"
#include "App/ServerConfig.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::server {

/**
 * @brief Unified Zappy server: one listening port, two protocols.
 *
 * Every connection starts with the WELCOME handshake. The team name then
 * routes the connection to the GUI protocol (team name "GRAPHIC") or the AI
 * protocol (any other team name). Both protocol opcode sets are registered on
 * the same dispatcher; no opcode collides between them. No game logic is wired
 * yet: handlers are protocol shells.
 */
class GameServer {
 public:
  explicit GameServer(const ServerConfig &config);

  /**
   * @brief Bind and listen on the configured port.
   */
  void start();

  /**
   * @brief Enter the main loop until @ref stop is called.
   *
   * Each iteration blocks in poll() for at most the delay until the next
   * scheduled game event (or indefinitely when none is pending), then executes
   * whatever events became due. The wake-up therefore happens only on socket
   * activity or on an event deadline — never by busy-waiting.
   */
  void run();

  /**
   * @brief Request the main loop to exit after the current iteration.
   */
  void stop();

  /**
   * @brief Return the port the server is listening on.
   */
  [[nodiscard]] int port() const;

 private:
  void registerHandshake();
  void registerGuiHandlers();
  void registerAiHandlers();
  void registerFallbacks();

  ServerConfig config_;
  zappy::rpc::RPCServer<ClientContext> server_;
  Scheduler scheduler_;
  bool running_{false};
};

}  // namespace zappy::server
