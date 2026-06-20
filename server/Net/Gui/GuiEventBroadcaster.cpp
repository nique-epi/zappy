/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiEventBroadcaster — pushes live game events to every connected GUI client
*/

#include "Net/Gui/GuiEventBroadcaster.hpp"
#include <string>
#include <vector>
#include "App/World/Player/Player.hpp"
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Team/Egg.hpp"
#include "App/World/Tile/Tile.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

using GuiSession = zappy::rpc::Session<ClientContext>;

namespace {

std::string playerTag(int playerId) { return "#" + std::to_string(playerId); }

std::string orientationOf(const world::Player& player) {
  return std::to_string(static_cast<int>(player.direction()));
}

void appendInventory(std::string& line, const world::Player& player) {
  for (const world::ResourceType type : world::allResourceTypes()) {
    line += " " + std::to_string(player.inventory().quantityOf(type));
  }
}

void appendTileContent(std::string& line, const world::Tile& tile) {
  for (const world::ResourceType type : world::allResourceTypes()) {
    line += " " + std::to_string(tile.quantityOf(type));
  }
}

}  // namespace

GuiEventBroadcaster::GuiEventBroadcaster(
    zappy::rpc::RPCServer<ClientContext>& server)
    : server_(server) {}

void GuiEventBroadcaster::playerConnected(const world::Player& player) {
  broadcast("pnw " + playerTag(player.id()) + " " + std::to_string(player.x()) +
            " " + std::to_string(player.y()) + " " + orientationOf(player) +
            " " + std::to_string(player.level()) + " " + player.teamName());
}

void GuiEventBroadcaster::playerMoved(const world::Player& player) {
  broadcast("ppo " + playerTag(player.id()) + " " + std::to_string(player.x()) +
            " " + std::to_string(player.y()) + " " + orientationOf(player));
}

void GuiEventBroadcaster::playerLevel(const world::Player& player) {
  broadcast("plv " + playerTag(player.id()) + " " +
            std::to_string(player.level()));
}

void GuiEventBroadcaster::playerInventory(const world::Player& player) {
  std::string line = "pin " + playerTag(player.id()) + " " +
                     std::to_string(player.x()) + " " +
                     std::to_string(player.y());
  appendInventory(line, player);
  broadcast(line);
}

void GuiEventBroadcaster::playerDied(int playerId) {
  broadcast("pdi " + playerTag(playerId));
}

void GuiEventBroadcaster::resourceCollected(int playerId,
                                            world::ResourceType resource) {
  broadcast("pgt " + playerTag(playerId) + " " +
            std::to_string(static_cast<int>(resource)));
}

void GuiEventBroadcaster::resourceDropped(int playerId,
                                          world::ResourceType resource) {
  broadcast("pdr " + playerTag(playerId) + " " +
            std::to_string(static_cast<int>(resource)));
}

void GuiEventBroadcaster::tileChanged(int column, int row,
                                      const world::Tile& tile) {
  std::string line =
      "bct " + std::to_string(column) + " " + std::to_string(row);
  appendTileContent(line, tile);
  broadcast(line);
}

void GuiEventBroadcaster::playerBroadcast(int playerId,
                                          const std::string& text) {
  broadcast("pbc " + playerTag(playerId) + " " + text);
}

void GuiEventBroadcaster::playerExpelled(int playerId) {
  broadcast("pex " + playerTag(playerId));
}

void GuiEventBroadcaster::incantationStarted(
    int column, int row, int level, const std::vector<int>& participants) {
  std::string line = "pic " + std::to_string(column) + " " +
                     std::to_string(row) + " " + std::to_string(level);
  for (const int participantId : participants) {
    line += " " + playerTag(participantId);
  }
  broadcast(line);
}

void GuiEventBroadcaster::incantationEnded(int column, int row, bool success) {
  broadcast("pie " + std::to_string(column) + " " + std::to_string(row) + " " +
            std::to_string(success ? 1 : 0));
}

void GuiEventBroadcaster::eggLaid(int parentPlayerId, const world::Egg& egg) {
  broadcast("pfk " + playerTag(parentPlayerId));
  broadcast("enw " + playerTag(egg.id) + " " + playerTag(parentPlayerId) + " " +
            std::to_string(egg.x) + " " + std::to_string(egg.y));
}

void GuiEventBroadcaster::eggHatched(int eggId) {
  broadcast("ebo " + playerTag(eggId));
}

void GuiEventBroadcaster::eggDestroyed(int eggId) {
  broadcast("edi " + playerTag(eggId));
}

void GuiEventBroadcaster::endGame(const std::string& teamName) {
  broadcast("seg " + teamName);
}

void GuiEventBroadcaster::serverMessage(const std::string& text) {
  broadcast("smg " + text);
}

void GuiEventBroadcaster::broadcast(const std::string& line) {
  server_.forEachSession([&line](GuiSession& session) {
    if (session.ctx().type == ClientType::Gui) {
      session.send(line);
    }
  });
}

}  // namespace zappy::server
