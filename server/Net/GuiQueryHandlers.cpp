/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiQueryHandlers — handlers replying to GUI map and team queries
*/

#include "Net/GuiQueryHandlers.hpp"
#include <charconv>
#include <string>
#include <string_view>
#include <system_error>
#include "App/World/Player/Player.hpp"
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Tile/Tile.hpp"
#include "Protocol/GuiProtocol.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

using GuiSession = zappy::rpc::Session<ClientContext>;
using GuiServer = zappy::rpc::RPCServer<ClientContext>;

namespace {

bool parseCoordinate(const std::string& text, int& out) {
  const std::string_view view{text};
  const auto* begin = view.data();
  const auto* end = begin + view.size();
  const auto result = std::from_chars(begin, end, out);
  return result.ec == std::errc{} && result.ptr == end;
}

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

bool parsePlayerNumber(const std::string& token, int& out) {
  if (token.size() < 2 || token.front() != '#') {
    return false;
  }
  const char* begin = token.data() + 1;
  const char* end = token.data() + token.size();
  const auto result = std::from_chars(begin, end, out);
  return result.ec == std::errc{} && result.ptr == end;
}

std::string formatPlayerInventory(int playerId, const world::Player& player) {
  std::string line = "pin #" + std::to_string(playerId) + " " +
                     std::to_string(player.x()) + " " +
                     std::to_string(player.y());
  for (const world::ResourceType type : world::allResourceTypes()) {
    line += " " + std::to_string(player.inventory().quantityOf(type));
  }
  return line;
}

void sendPlayerPosition(GuiSession& session, int playerId,
                        const world::Player& player) {
  session.send("ppo #" + std::to_string(playerId) + " " +
               std::to_string(player.x()) + " " + std::to_string(player.y()) +
               " " + std::to_string(static_cast<int>(player.direction())));
}

void sendPlayerLevel(GuiSession& session, int playerId,
                     const world::Player& player) {
  session.send("plv #" + std::to_string(playerId) + " " +
               std::to_string(player.level()));
}

const world::Player* resolvePlayer(GuiSession& session,
                                   const protocol::PlayerNumberArgs& args,
                                   const world::PlayerRegistry& players,
                                   int& playerId) {
  if (!parsePlayerNumber(args.playerNumber, playerId)) {
    session.send(protocol::BadParameter().opcode());
    return nullptr;
  }
  const world::Player* player = players.find(playerId);
  if (player == nullptr) {
    session.send(protocol::BadParameter().opcode());
  }
  return player;
}

}  // namespace

void installGuiQueryHandlers(GuiServer& server, const ServerConfig& config,
                             const world::Map& map,
                             const world::PlayerRegistry& players,
                             TimeUnit& timeUnit) {
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
              int column = 0;
              int row = 0;
              if (!parseCoordinate(args.positionX, column) ||
                  !parseCoordinate(args.positionY, row)) {
                session.send(protocol::BadParameter().opcode());
                return;
              }
              sendTileContent(session, column, row, map);
            });

  server.on(protocol::RequestMapContent(),
            [&map](GuiSession& session, zappy::rpc::IMessage&) {
              sendMapContent(session, map);
            });

  server.on(
      protocol::RequestPlayerPosition(),
      [&players](GuiSession& session, const protocol::PlayerNumberArgs& args) {
        int playerId = 0;
        const world::Player* player =
            resolvePlayer(session, args, players, playerId);
        if (player != nullptr) {
          sendPlayerPosition(session, playerId, *player);
        }
      });

  server.on(
      protocol::RequestPlayerLevel(),
      [&players](GuiSession& session, const protocol::PlayerNumberArgs& args) {
        int playerId = 0;
        const world::Player* player =
            resolvePlayer(session, args, players, playerId);
        if (player != nullptr) {
          sendPlayerLevel(session, playerId, *player);
        }
      });

  server.on(
      protocol::RequestPlayerInventory(),
      [&players](GuiSession& session, const protocol::PlayerNumberArgs& args) {
        int playerId = 0;
        const world::Player* player =
            resolvePlayer(session, args, players, playerId);
        if (player != nullptr) {
          session.send(formatPlayerInventory(playerId, *player));
        }
      });

  server.on(protocol::RequestTimeUnit(),
            [&timeUnit](GuiSession& session, zappy::rpc::IMessage&) {
              session.send("sgt " + std::to_string(timeUnit.value()));
            });

  server.on(
      protocol::SetTimeUnit(),
      [&timeUnit](GuiSession& session, const protocol::TimeUnitArgs& args) {
        int frequency = 0;
        if (!parseCoordinate(args.timeUnit, frequency) || frequency <= 0) {
          session.send(protocol::BadParameter().opcode());
          return;
        }
        timeUnit.set(frequency);
        session.send("sst " + std::to_string(frequency));
      });
}

}  // namespace zappy::server
