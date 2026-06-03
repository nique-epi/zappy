/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GameServer — single-port server hosting both GUI and AI protocols
*/

#include "App/GameServer.hpp"
#include <string>
#include "Protocol/AiProtocol.hpp"
#include "Protocol/GuiProtocol.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

namespace {
constexpr const char *graphicTeamName = "GRAPHIC";
constexpr const char *welcomeGreeting = "WELCOME";
}  // namespace

using Session = zappy::rpc::Session<ClientContext>;

GameServer::GameServer(const ServerConfig &config)
    : config_(config), server_(config.port) {
  registerHandshake();
  registerGuiHandlers();
  registerAiHandlers();
  registerFallbacks();
}

void GameServer::start() { server_.start(); }

void GameServer::run(int timeoutMs) { server_.run(timeoutMs); }

int GameServer::port() const { return server_.port(); }

void GameServer::registerHandshake() {
  server_.onConnect([](Session &session) { session.send(welcomeGreeting); });

  server_.onHandshake([this](Session &session, const std::string &teamName) {
    if (teamName == graphicTeamName) {
      session.ctx().type = ClientType::Gui;
      session.completeHandshake();
      return;
    }
    session.ctx().type = ClientType::Ai;
    session.ctx().teamName = teamName;
    session.send(std::to_string(config_.clientsPerTeam));
    session.send(std::to_string(config_.width) + " " +
                 std::to_string(config_.height));
    session.completeHandshake();
  });
}

void GameServer::registerGuiHandlers() {
  server_.on(protocol::RequestMapSize(),
             [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::RequestMapContent(),
             [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::RequestTeamNames(),
             [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::RequestTimeUnit(),
             [](Session &, zappy::rpc::IMessage &) {});

  server_.on(protocol::RequestTileContent(),
             [](Session &, const protocol::TileRequestArgs &) {});
  server_.on(protocol::RequestPlayerPosition(),
             [](Session &, const protocol::PlayerNumberArgs &) {});
  server_.on(protocol::RequestPlayerLevel(),
             [](Session &, const protocol::PlayerNumberArgs &) {});
  server_.on(protocol::RequestPlayerInventory(),
             [](Session &, const protocol::PlayerNumberArgs &) {});
  server_.on(protocol::SetTimeUnit(),
             [](Session &, const protocol::TimeUnitArgs &) {});
}

void GameServer::registerAiHandlers() {
  server_.on(protocol::ai::Forward(), [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::ai::Right(), [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::ai::Left(), [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::ai::Look(), [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::ai::Inventory(),
             [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::ai::ConnectNbr(),
             [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::ai::Fork(), [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::ai::Eject(), [](Session &, zappy::rpc::IMessage &) {});
  server_.on(protocol::ai::Incantation(),
             [](Session &, zappy::rpc::IMessage &) {});

  server_.on(protocol::ai::Take(),
             [](Session &, const protocol::ai::ObjectArgs &) {});
  server_.on(protocol::ai::Set(),
             [](Session &, const protocol::ai::ObjectArgs &) {});
  server_.on(protocol::ai::Broadcast(),
             [](Session &, const protocol::ai::BroadcastTextArgs &) {});
}

void GameServer::registerFallbacks() {
  server_.onUnknown([](Session &session, zappy::rpc::IMessage &) {
    if (session.ctx().type == ClientType::Ai) {
      session.send(protocol::ai::Ko().opcode());
      return;
    }
    session.send(protocol::UnknownCommand().opcode());
  });

  server_.onInvalid([](Session &session, zappy::rpc::IMessage &) {
    if (session.ctx().type == ClientType::Ai) {
      session.send(protocol::ai::Ko().opcode());
      return;
    }
    session.send(protocol::BadParameter().opcode());
  });
}

}  // namespace zappy::server
