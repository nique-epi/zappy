/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiQueryHandlers — handlers replying to GUI map and team queries
*/

#include "Net/GuiQueryHandlers.hpp"
#include <string>
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Tile/Tile.hpp"
#include "Protocol/GuiProtocol.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

using GuiSession = zappy::rpc::Session<ClientContext>;
using GuiServer = zappy::rpc::RPCServer<ClientContext>;

namespace {

std::string formatTileContent(int column, int row, const world::Tile& tile) {
  std::string line =
      "bct " + std::to_string(column) + " " + std::to_string(row);
  for (const world::ResourceType type : world::allResourceTypes()) {
    line += " " + std::to_string(tile.quantityOf(type));
  }
  return line;
}

void sendMapSize(GuiSession& session, const ServerConfig& config) {
  session.send("msz " + std::to_string(config.width) + " " +
               std::to_string(config.height));
}

void sendTeamNames(GuiSession& session, const ServerConfig& config) {
  for (const std::string& teamName : config.teamNames) {
    session.send("tna " + teamName);
  }
}

void sendTileContent(GuiSession& session, int column, int row,
                     const world::Map& map) {
  session.send(formatTileContent(column, row, map.tileAt(column, row)));
}

void sendMapContent(GuiSession& session, const world::Map& map) {
  for (int row = 0; row < map.height(); row++) {
    for (int column = 0; column < map.width(); column++) {
      sendTileContent(session, column, row, map);
    }
  }
}

}  // namespace

void installGuiQueryHandlers(GuiServer& server, const ServerConfig& config,
                             const world::Map& map) {
  server.on(protocol::RequestMapSize(),
            [&config](GuiSession& session, zappy::rpc::IMessage&) {
              sendMapSize(session, config);
            });

  server.on(protocol::RequestTeamNames(),
            [&config](GuiSession& session, zappy::rpc::IMessage&) {
              sendTeamNames(session, config);
            });

  server.on(protocol::RequestTileContent(),
            [&map](GuiSession& session, const protocol::TileRequestArgs& args) {
              sendTileContent(session, std::stoi(args.positionX),
                              std::stoi(args.positionY), map);
            });

  server.on(protocol::RequestMapContent(),
            [&map](GuiSession& session, zappy::rpc::IMessage&) {
              sendMapContent(session, map);
            });
}

}  // namespace zappy::server
