/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GameServer — single-port server hosting both GUI and AI protocols
*/

#include "App/GameServer.hpp"
#include <random>
#include <string>
#include "App/World/Ejection.hpp"
#include "App/World/Exceptions/WorldException.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Team/Egg.hpp"
#include "Net/AiActionPipeline.hpp"
#include "Net/GuiQueryHandlers.hpp"
#include "Protocol/AiProtocol.hpp"
#include "Protocol/GuiProtocol.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

namespace {
constexpr const char* graphicTeamName = "GRAPHIC";
constexpr const char* welcomeGreeting = "WELCOME";
}  // namespace

using Session = zappy::rpc::Session<ClientContext>;

GameServer::GameServer(const ServerConfig& config)
    : config_(config),
      server_(config.port),
      world_(config.width, config.height),
      rng_(std::random_device{}()),
      teams_(config.teamNames),
      loot_(world_, config.frequency, std::random_device{}()),
      hunger_(server_, scheduler_, players_, world_, config.frequency) {
  teams_.seedInitialEggs(world_, config_.clientsPerTeam, rng_);
  registerHandshake();
  registerGuiHandlers();
  registerAiHandlers();
  registerFallbacks();
  installAiActionPipeline(server_, scheduler_, config_.frequency);
  installGuiQueryHandlers(server_, config_, world_);
  server_.onDisconnect([this](Session& session) {
    session.ctx().pendingActions.clear();
    session.ctx().actionInFlight = false;
    if (session.ctx().playerId != 0) {
      players_.remove(session.ctx().playerId, world_);
      session.ctx().playerId = 0;
    }
  });
}

void GameServer::start() { server_.start(); }

void GameServer::run() {
  running_ = true;
  loot_.start(scheduler_, Scheduler::Clock::now());
  while (running_) {
    server_.runOnce(scheduler_.timeoutUntilNext(Scheduler::Clock::now()));
    scheduler_.runDue(Scheduler::Clock::now());
  }
}

void GameServer::stop() { running_ = false; }

int GameServer::port() const { return server_.port(); }

void GameServer::registerHandshake() {
  server_.onConnect([](Session& session) { session.send(welcomeGreeting); });

  server_.onHandshake([this](Session& session, const std::string& teamName) {
    if (teamName == graphicTeamName) {
      session.ctx().type = ClientType::Gui;
      session.completeHandshake();
      return;
    }
    try {
      const world::Egg hatched = teams_.hatch(teamName, world_, rng_);
      const world::Direction direction =
          world::PlayerRegistry::randomDirection(rng_);
      const world::Player& player =
          players_.spawn(teamName, hatched.x, hatched.y, direction, world_);
      session.ctx().type = ClientType::Ai;
      session.ctx().teamName = teamName;
      session.ctx().spawnX = hatched.x;
      session.ctx().spawnY = hatched.y;
      session.ctx().playerId = player.id();
      session.send(std::to_string(teams_.freeSlots(teamName)));
      session.send(std::to_string(config_.width) + " " +
                   std::to_string(config_.height));
      session.completeHandshake();
      hunger_.armFirstTick(session.fd(), Scheduler::Clock::now());
    } catch (const world::WorldException&) {
      session.send(protocol::ai::Ko().opcode());
    }
  });
}

void GameServer::registerGuiHandlers() {
  server_.on(protocol::RequestTimeUnit(),
             [](Session&, zappy::rpc::IMessage&) {});

  server_.on(protocol::RequestPlayerPosition(),
             [](Session&, const protocol::PlayerNumberArgs&) {});
  server_.on(protocol::RequestPlayerLevel(),
             [](Session&, const protocol::PlayerNumberArgs&) {});
  server_.on(protocol::RequestPlayerInventory(),
             [](Session&, const protocol::PlayerNumberArgs&) {});
  server_.on(protocol::SetTimeUnit(),
             [](Session&, const protocol::TimeUnitArgs&) {});
}

void GameServer::registerAiHandlers() {
  server_.on(protocol::ai::Forward(), [](Session&, zappy::rpc::IMessage&) {});
  server_.on(protocol::ai::Right(), [](Session&, zappy::rpc::IMessage&) {});
  server_.on(protocol::ai::Left(), [](Session&, zappy::rpc::IMessage&) {});
  server_.on(protocol::ai::Look(), [](Session&, zappy::rpc::IMessage&) {});
  server_.on(protocol::ai::Inventory(), [](Session&, zappy::rpc::IMessage&) {});
  server_.on(protocol::ai::ConnectNbr(),
             [](Session&, zappy::rpc::IMessage&) {});
  server_.on(protocol::ai::Fork(),
             [this](Session& session, zappy::rpc::IMessage&) {
               executeFork(session);
             });
  server_.on(protocol::ai::Eject(),
             [this](Session& session, zappy::rpc::IMessage&) {
               executeEject(session);
             });
  server_.on(protocol::ai::Incantation(),
             [](Session&, zappy::rpc::IMessage&) {});

  server_.on(protocol::ai::Take(),
             [](Session&, const protocol::ai::ObjectArgs&) {});
  server_.on(protocol::ai::Set(),
             [](Session&, const protocol::ai::ObjectArgs&) {});
  server_.on(protocol::ai::Broadcast(),
             [](Session&, const protocol::ai::BroadcastTextArgs&) {});
}

void GameServer::executeFork(Session& session) {
  world::Player* player = players_.find(session.ctx().playerId);
  if (player == nullptr) {
    session.send(protocol::ai::Ko().opcode());
    return;
  }
  teams_.lay(player->teamName(), world_, player->x(), player->y());
  session.send(protocol::ai::Ok().opcode());
}

void GameServer::executeEject(Session& session) {
  const int ejectorId = session.ctx().playerId;
  const world::EjectOutcome outcome =
      world::ejectFromTile(ejectorId, players_, world_, teams_);
  for (const world::EjectedDrone& drone : outcome.ejected) {
    const std::string line = "eject: " + std::to_string(drone.code);
    server_.forEachSession([&drone, &line](Session& other) {
      if (other.ctx().playerId == drone.playerId) {
        other.send(line);
      }
    });
  }
  session.send(outcome.ejected.empty() ? protocol::ai::Ko().opcode()
                                       : protocol::ai::Ok().opcode());
}

void GameServer::registerFallbacks() {
  server_.onUnknown([](Session& session, zappy::rpc::IMessage&) {
    if (session.ctx().type == ClientType::Ai) {
      session.send(protocol::ai::Ko().opcode());
      return;
    }
    session.send(protocol::UnknownCommand().opcode());
  });

  server_.onInvalid([](Session& session, zappy::rpc::IMessage&) {
    if (session.ctx().type == ClientType::Ai) {
      session.send(protocol::ai::Ko().opcode());
      return;
    }
    session.send(protocol::BadParameter().opcode());
  });
}

}  // namespace zappy::server
