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
#include "App/World/Map/Map.hpp"
#include "App/World/Player/HungerService.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "App/World/Team/TeamRegistry.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::protocol::ai {
struct BroadcastTextArgs;
}  // namespace zappy::protocol::ai

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
  void registerGuiHandlers();
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
   * @brief Lay an egg on the calling drone's tile (Fork): a new free slot for
   *        its team. Replies `ok`, or `ko` if the session owns no drone.
   */
  void executeFork(zappy::rpc::Session<ClientContext>& session);

  /**
   * @brief Push every other drone off the calling drone's tile (Eject),
   *        destroying the eggs on it.
   *
   * Each pushed drone is told `eject: K` with its incoming direction. Replies
   * `ok` when at least one drone was pushed, `ko` otherwise.
   */
  void executeEject(zappy::rpc::Session<ClientContext>& session);

  /**
   * @brief Broadcast @p args text to every drone (Broadcast).
   *
   * Replies `ok` to the emitter and pushes `message K, text` to every AI
   * client, where K is the tile the sound reaches that listener from, computed
   * per receiver along the shortest toroidal path. The emitter stays anonymous.
   * Replies `ko` when the session owns no drone.
   */
  void executeBroadcast(zappy::rpc::Session<ClientContext>& session,
                        const protocol::ai::BroadcastTextArgs& args);

  ServerConfig config_;
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
