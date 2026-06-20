/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** HungerService — ticks the food reserve of every AI session and kills the
**                 starving ones
*/

#pragma once

#include "App/Scheduler/Scheduler.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::server {

class GuiEventBroadcaster;

/// Game time units a single food unit is worth (subject rule: 126/f seconds).
constexpr int gameTicksPerFoodUnit = 126;

/**
 * @brief Drives the hunger / death loop without active waiting.
 *
 * For every drone the service arms a one-shot Scheduler event at @c now + 126/f
 * seconds. When the deadline fires the drone's food reserve is decremented; if
 * any food is left the next tick is re-armed, otherwise the drone is declared
 * dead: a `dead` line is sent to the AI session, the drone is removed from the
 * world (registry + tile) and the session's player id is cleared so further
 * commands no longer route to it.
 *
 * The service never reads the clock itself — every deadline is computed from a
 * caller-supplied @ref Scheduler::TimePoint — keeping its behaviour fully
 * deterministic and testable without sleeping.
 */
class HungerService {
 public:
  HungerService(zappy::rpc::RPCServer<ClientContext>& server,
                Scheduler& scheduler, world::PlayerRegistry& players,
                world::Map& map, GuiEventBroadcaster& gui, int frequency);

  /**
   * @brief Arm the first food tick for the AI session backing @p
   * fileDescriptor.
   * @param[in] fileDescriptor Session fd; ignored if the session no longer
   *                           exists.
   * @param[in] now            Reference time the tick deadline is computed
   *                           from.
   */
  void armFirstTick(int fileDescriptor, Scheduler::TimePoint now);

 private:
  void scheduleTick(int fileDescriptor, int playerId,
                    Scheduler::TimePoint from);
  void onTick(int fileDescriptor, int playerId, Scheduler::TimePoint deadline);
  void killPlayer(zappy::rpc::Session<ClientContext>& session);

  zappy::rpc::RPCServer<ClientContext>& server_;
  Scheduler& scheduler_;
  world::PlayerRegistry& players_;
  world::Map& map_;
  GuiEventBroadcaster& gui_;
  int frequency_;
};

}  // namespace zappy::server
