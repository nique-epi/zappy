/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GameServer — single-port server hosting both GUI and AI protocols
*/

#pragma once

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
   * @brief Enter the poll loop.
   * @param timeoutMs Poll wake-up timeout; -1 blocks until socket activity.
   */
  void run(int timeoutMs = -1);

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
};

}  // namespace zappy::server
