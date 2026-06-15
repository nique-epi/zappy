/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** AiHandlerSupport — helpers shared by the AI command handlers
*/

#include "Net/Ai/AiHandlerSupport.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "Protocol/AiProtocol.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

world::Player* findPlayerOrReplyKo(AiSession& session,
                                   const AiHandlerContext& context) {
  world::Player* player = context.players.find(session.ctx().playerId);
  if (player == nullptr) {
    session.send(protocol::ai::Ko().opcode());
  }
  return player;
}

}  // namespace zappy::server
