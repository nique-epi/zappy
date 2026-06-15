/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ReproductionHandlers — AI Fork/Eject command handlers
*/

#include "Net/Ai/Reproduction/ReproductionHandlers.hpp"
#include <string>
#include "App/World/Ejection.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Team/TeamRegistry.hpp"
#include "Net/Ai/AiHandlerSupport.hpp"
#include "Protocol/AiProtocol.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

namespace {

void handleFork(AiSession& session, const AiHandlerContext& context) {
  world::Player* player = findPlayerOrReplyKo(session, context);
  if (player == nullptr) {
    return;
  }
  context.teams.lay(player->teamName(), context.map, player->x(), player->y());
  session.send(protocol::ai::Ok().opcode());
}

void handleEject(AiServer& server, AiSession& session,
                 const AiHandlerContext& context) {
  const int ejectorId = session.ctx().playerId;
  const world::EjectOutcome outcome = world::ejectFromTile(
      ejectorId, context.players, context.map, context.teams);
  for (const world::EjectedDrone& drone : outcome.ejected) {
    const std::string line = "eject: " + std::to_string(drone.code);
    server.forEachSession([&drone, &line](AiSession& other) {
      if (other.ctx().playerId == drone.playerId) {
        other.send(line);
      }
    });
  }
  session.send(outcome.ejected.empty() ? protocol::ai::Ko().opcode()
                                       : protocol::ai::Ok().opcode());
}

}  // namespace

void installReproductionHandlers(AiServer& server,
                                 const AiHandlerContext& context) {
  server.on(protocol::ai::Fork(),
            [context](AiSession& session, zappy::rpc::IMessage&) {
              handleFork(session, context);
            });
  server.on(protocol::ai::Eject(),
            [&server, context](AiSession& session, zappy::rpc::IMessage&) {
              handleEject(server, session, context);
            });
}

}  // namespace zappy::server
