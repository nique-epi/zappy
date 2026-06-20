/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceHandlers — AI Take/Set command handlers
*/

#include "Net/Ai/Resources/ResourceHandlers.hpp"
#include <optional>
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/ResourceTransfer.hpp"
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Tile/Tile.hpp"
#include "Net/Ai/AiHandlerSupport.hpp"
#include "Net/Gui/GuiEventBroadcaster.hpp"
#include "Protocol/AiProtocol.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

namespace {

void handleTake(AiSession& session, const protocol::ai::ObjectArgs& args,
                const AiHandlerContext& context) {
  world::Player* player = findPlayerOrReplyKo(session, context);
  if (player == nullptr) {
    return;
  }
  const std::optional<world::ResourceType> type =
      world::resourceFromName(args.object);
  if (!type.has_value()) {
    session.send(protocol::ai::Ko().opcode());
    return;
  }
  world::Tile& tile = context.map.tileAt(player->x(), player->y());
  if (!world::takeResource(*player, tile, *type)) {
    session.send(protocol::ai::Ko().opcode());
    return;
  }
  context.gui.resourceCollected(player->id(), *type);
  context.gui.playerInventory(*player);
  session.send(protocol::ai::Ok().opcode());
}

void handleSet(AiSession& session, const protocol::ai::ObjectArgs& args,
               const AiHandlerContext& context) {
  world::Player* player = findPlayerOrReplyKo(session, context);
  if (player == nullptr) {
    return;
  }
  const std::optional<world::ResourceType> type =
      world::resourceFromName(args.object);
  if (!type.has_value()) {
    session.send(protocol::ai::Ko().opcode());
    return;
  }
  world::Tile& tile = context.map.tileAt(player->x(), player->y());
  if (!world::dropResource(*player, tile, *type)) {
    session.send(protocol::ai::Ko().opcode());
    return;
  }
  context.gui.resourceDropped(player->id(), *type);
  context.gui.playerInventory(*player);
  session.send(protocol::ai::Ok().opcode());
}

}  // namespace

void installResourceHandlers(AiServer& server,
                             const AiHandlerContext& context) {
  server.on(
      protocol::ai::Take(),
      [context](AiSession& session, const protocol::ai::ObjectArgs& args) {
        handleTake(session, args, context);
      });
  server.on(
      protocol::ai::Set(),
      [context](AiSession& session, const protocol::ai::ObjectArgs& args) {
        handleSet(session, args, context);
      });
}

}  // namespace zappy::server
