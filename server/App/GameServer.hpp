/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GameServer — single-port server hosting both GUI and AI protocols
*/

#pragma once

#include <random>
#include <string>
#include "App/Loot/LootService.hpp"
#include "App/Scheduler/Scheduler.hpp"
#include "App/ServerConfig.hpp"
#include "App/TimeUnit.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Player/HungerService.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "App/World/Team/TeamRegistry.hpp"
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
  explicit GameServer(const ServerConfig& config);

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
   * @brief Run a single poll/dispatch iteration: block in poll() for at most
   *        @p timeoutMs, then execute whatever scheduled events became due.
   *
   * This is the body of @ref run, exposed so callers (and tests) can drive the
   * server one step at a time without owning the loop.
   *
   * @param[in] timeoutMs Maximum poll() block in milliseconds.
   */
  void runOnce(int timeoutMs);

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
  void registerAiHandlers();
  void registerFallbacks();

  /**
   * @brief Run the AI side of the handshake for a known team.
   *
   * A free slot hatches one of the team's eggs, spawns the drone, replies with
   * the remaining slot count then the world size, and arms the hunger loop. A
   * full team replies with `0` slots then the world size and admits no drone:
   * no egg is consumed, no player is spawned.
   *
   * @param[in,out] session  Session whose handshake is being resolved.
   * @param[in]     teamName  Known team the client asked to join.
   */
  void admitAiClient(zappy::rpc::Session<ClientContext>& session,
                     const std::string& teamName);

  /// World dimensions formatted as the `X Y` handshake line.
  [[nodiscard]] std::string worldSizeLine() const;

  /**
   * @brief Resolve the elevation ritual started by the calling drone
   *        (Incantation).
   *
   * The action's 300/f cost already froze the initiator for the ritual's
   * duration, so this runs at the end of that window: it verifies the
   * prerequisites on the tile and, on success, removes the minerals and raises
   * every gathered drone, sending each `Current level: k`. On failure the
   * initiator is told `ko`.
   */
  void executeIncantation(zappy::rpc::Session<ClientContext>& session);

  ServerConfig config_;
  TimeUnit timeUnit_;
  zappy::rpc::RPCServer<ClientContext> server_;
  world::Map world_;
  std::mt19937 rng_;
  world::TeamRegistry teams_;
  world::PlayerRegistry players_;
  loot::LootService loot_;
  Scheduler scheduler_;
  HungerService hunger_;
  bool running_{false};
};

}  // namespace zappy::server
