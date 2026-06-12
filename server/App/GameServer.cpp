/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GameServer — single-port server hosting both GUI and AI protocols
*/

#include "App/GameServer.hpp"
#include <random>
#include <string>
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Team/Egg.hpp"
#include "Net/Ai/AiHandlerContext.hpp"
#include "Net/Ai/Movement/MovementHandlers.hpp"
#include "Net/Ai/Observation/ObservationHandlers.hpp"
#include "Net/Ai/Reproduction/ReproductionHandlers.hpp"
#include "Net/Ai/Resources/ResourceHandlers.hpp"
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
    runOnce(scheduler_.timeoutUntilNext(Scheduler::Clock::now()));
  }
}

void GameServer::runOnce(int timeoutMs) {
  server_.runOnce(timeoutMs);
  scheduler_.runDue(Scheduler::Clock::now());
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
    if (!teams_.contains(teamName)) {
      session.send(protocol::ai::Ko().opcode());
      return;
    }
    admitAiClient(session, teamName);
  });
}

void GameServer::admitAiClient(Session& session, const std::string& teamName) {
  session.ctx().type = ClientType::Ai;
  session.ctx().teamName = teamName;
  if (teams_.freeSlots(teamName) == 0) {
    session.send("0");
    session.send(worldSizeLine());
    session.completeHandshake();
    return;
  }
  const world::Egg hatched = teams_.hatch(teamName, world_, rng_);
  const world::Direction direction =
      world::PlayerRegistry::randomDirection(rng_);
  const world::Player& player =
      players_.spawn(teamName, hatched.x, hatched.y, direction, world_);
  session.ctx().spawnX = hatched.x;
  session.ctx().spawnY = hatched.y;
  session.ctx().playerId = player.id();
  session.send(std::to_string(teams_.freeSlots(teamName)));
  session.send(worldSizeLine());
  session.completeHandshake();
  hunger_.armFirstTick(session.fd(), Scheduler::Clock::now());
}

std::string GameServer::worldSizeLine() const {
  return std::to_string(config_.width) + " " + std::to_string(config_.height);
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
  const AiHandlerContext context{
      .players = players_, .map = world_, .teams = teams_};
  installMovementHandlers(server_, context);
  installResourceHandlers(server_, context);
  installObservationHandlers(server_, context);
  installReproductionHandlers(server_, context);
  server_.on(protocol::ai::Incantation(),
             [](Session&, zappy::rpc::IMessage&) {});
  server_.on(protocol::ai::Broadcast(),
             [](Session&, const protocol::ai::BroadcastTextArgs&) {});
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
