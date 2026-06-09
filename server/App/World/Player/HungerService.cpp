/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** HungerService — ticks the food reserve of every AI session and kills the
**                 starving ones
*/

#include "App/World/Player/HungerService.hpp"
#include "App/Scheduler/ActionTiming.hpp"
#include "App/World/Player/Player.hpp"
#include "Protocol/AiProtocol.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

using AiSession = zappy::rpc::Session<ClientContext>;

HungerService::HungerService(zappy::rpc::RPCServer<ClientContext>& server,
                             Scheduler& scheduler,
                             world::PlayerRegistry& players, world::Map& map,
                             int frequency)
    : server_(server),
      scheduler_(scheduler),
      players_(players),
      map_(map),
      frequency_(frequency) {}

void HungerService::armFirstTick(int fileDescriptor, Scheduler::TimePoint now) {
  AiSession* session = server_.session(fileDescriptor);
  if (session == nullptr || session->ctx().playerId == 0) {
    return;
  }
  scheduleTick(fileDescriptor, now);
}

void HungerService::scheduleTick(int fileDescriptor, Scheduler::TimePoint now) {
  const Scheduler::TimePoint deadline =
      now + actionDuration(gameTicksPerFoodUnit, frequency_);
  scheduler_.scheduleAt(deadline,
                        [this, fileDescriptor]() { onTick(fileDescriptor); });
}

void HungerService::onTick(int fileDescriptor) {
  AiSession* session = server_.session(fileDescriptor);
  if (session == nullptr) {
    return;
  }
  const int playerId = session->ctx().playerId;
  if (playerId == 0) {
    return;
  }
  world::Player* player = players_.find(playerId);
  if (player == nullptr) {
    return;
  }
  player->inventory().consume(world::ResourceType::Food, 1);
  if (player->inventory().quantityOf(world::ResourceType::Food) == 0) {
    killPlayer(*session);
    return;
  }
  scheduleTick(fileDescriptor, Scheduler::Clock::now());
}

void HungerService::killPlayer(AiSession& session) {
  session.send(zappy::protocol::ai::Dead().opcode());
  const int playerId = session.ctx().playerId;
  players_.remove(playerId, map_);
  session.ctx().playerId = 0;
}

}  // namespace zappy::server
