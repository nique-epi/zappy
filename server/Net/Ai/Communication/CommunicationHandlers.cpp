/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** CommunicationHandlers — AI Broadcast command handler
*/

#include "Net/Ai/Communication/CommunicationHandlers.hpp"
#include <string>
#include "App/World/Broadcast.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "Net/Ai/AiHandlerSupport.hpp"
#include "Net/Gui/GuiEventBroadcaster.hpp"
#include "Protocol/AiProtocol.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

namespace {

void pushSoundToListener(AiSession& listener, int emitterX, int emitterY,
                         const std::string& text,
                         const AiHandlerContext& context) {
  if (listener.ctx().type != ClientType::Ai) {
    return;
  }
  const world::Player* receiver = context.players.find(listener.ctx().playerId);
  if (receiver == nullptr) {
    return;
  }
  const int code = world::broadcastSoundDirection(
      emitterX, emitterY, receiver->x(), receiver->y(), receiver->direction(),
      context.map.width(), context.map.height());
  listener.send("message " + std::to_string(code) + ", " + text);
}

void handleBroadcast(AiServer& server, AiSession& session,
                     const protocol::ai::BroadcastTextArgs& args,
                     const AiHandlerContext& context) {
  const world::Player* emitter = findPlayerOrReplyKo(session, context);
  if (emitter == nullptr) {
    return;
  }
  const int emitterX = emitter->x();
  const int emitterY = emitter->y();
  const int emitterId = emitter->id();
  const std::string& text = args.text;
  server.forEachSession([&](AiSession& listener) {
    pushSoundToListener(listener, emitterX, emitterY, text, context);
  });
  context.gui.playerBroadcast(emitterId, text);
  session.send(protocol::ai::Ok().opcode());
}

}  // namespace

void installCommunicationHandlers(AiServer& server,
                                  const AiHandlerContext& context) {
  server.on(protocol::ai::Broadcast(),
            [&server, context](AiSession& session,
                               const protocol::ai::BroadcastTextArgs& args) {
              handleBroadcast(server, session, args, context);
            });
}

}  // namespace zappy::server
