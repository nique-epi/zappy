/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ObservationHandlers — AI Look/Inventory/Connect_nbr command handlers
*/

#include "Net/Ai/Observation/ObservationHandlers.hpp"
#include <cstddef>
#include <string>
#include "App/World/Observation.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Team/TeamRegistry.hpp"
#include "Net/Ai/AiHandlerSupport.hpp"
#include "Protocol/AiProtocol.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

namespace {

void handleLook(AiSession& session, const AiHandlerContext& context) {
  const world::Player* player = findPlayerOrReplyKo(session, context);
  if (player == nullptr) {
    return;
  }
  session.send(world::formatLook(player->x(), player->y(), player->direction(),
                                 player->level(), context.map));
}

void handleInventory(AiSession& session, const AiHandlerContext& context) {
  const world::Player* player = findPlayerOrReplyKo(session, context);
  if (player == nullptr) {
    return;
  }
  session.send(world::formatInventory(player->inventory()));
}

void handleConnectNbr(AiSession& session, const AiHandlerContext& context) {
  const std::string& teamName = session.ctx().teamName;
  const std::size_t freeSlots =
      context.teams.contains(teamName) ? context.teams.freeSlots(teamName) : 0;
  session.send(std::to_string(freeSlots));
}

}  // namespace

void installObservationHandlers(AiServer& server,
                                const AiHandlerContext& context) {
  server.on(protocol::ai::Look(),
            [context](AiSession& session, zappy::rpc::IMessage&) {
              handleLook(session, context);
            });
  server.on(protocol::ai::Inventory(),
            [context](AiSession& session, zappy::rpc::IMessage&) {
              handleInventory(session, context);
            });
  server.on(protocol::ai::ConnectNbr(),
            [context](AiSession& session, zappy::rpc::IMessage&) {
              handleConnectNbr(session, context);
            });
}

}  // namespace zappy::server
