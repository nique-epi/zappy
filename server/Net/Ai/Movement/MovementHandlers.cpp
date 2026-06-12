/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MovementHandlers — AI Forward/Right/Left command handlers
*/

#include "Net/Ai/Movement/MovementHandlers.hpp"
#include "App/World/Movement.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "Protocol/AiProtocol.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

using AiSession = zappy::rpc::Session<ClientContext>;
using AiServer = zappy::rpc::RPCServer<ClientContext>;

namespace {

void handleForward(AiSession& session, const AiHandlerContext& context) {
  world::Player* player = context.players.find(session.ctx().playerId);
  if (player == nullptr) {
    session.send(protocol::ai::Ko().opcode());
    return;
  }
  world::moveForward(*player, context.map);
  session.send(protocol::ai::Ok().opcode());
}

void handleRight(AiSession& session, const AiHandlerContext& context) {
  world::Player* player = context.players.find(session.ctx().playerId);
  if (player == nullptr) {
    session.send(protocol::ai::Ko().opcode());
    return;
  }
  player->setDirection(world::turnRight(player->direction()));
  session.send(protocol::ai::Ok().opcode());
}

void handleLeft(AiSession& session, const AiHandlerContext& context) {
  world::Player* player = context.players.find(session.ctx().playerId);
  if (player == nullptr) {
    session.send(protocol::ai::Ko().opcode());
    return;
  }
  player->setDirection(world::turnLeft(player->direction()));
  session.send(protocol::ai::Ok().opcode());
}

}  // namespace

void installMovementHandlers(AiServer& server,
                             const AiHandlerContext& context) {
  server.on(protocol::ai::Forward(),
            [context](AiSession& session, zappy::rpc::IMessage&) {
              handleForward(session, context);
            });
  server.on(protocol::ai::Right(),
            [context](AiSession& session, zappy::rpc::IMessage&) {
              handleRight(session, context);
            });
  server.on(protocol::ai::Left(),
            [context](AiSession& session, zappy::rpc::IMessage&) {
              handleLeft(session, context);
            });
}

}  // namespace zappy::server
