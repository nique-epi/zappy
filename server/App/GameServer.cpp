/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GameServer — single-port server hosting both GUI and AI protocols
*/

#include "App/GameServer.hpp"
#include <optional>
#include <random>
#include <string>
#include "App/World/Incantation/Incantation.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Team/Egg.hpp"
#include "App/World/Victory.hpp"
#include "Net/Ai/AiHandlerContext.hpp"
#include "Net/Ai/Communication/CommunicationHandlers.hpp"
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
      timeUnit_(config.frequency),
      server_(config.port),
      gui_(server_),
      world_(config.width, config.height),
      rng_(std::random_device{}()),
      teams_(config.teamNames),
      loot_(world_, gui_, config.frequency, std::random_device{}()),
      hunger_(server_, scheduler_, players_, world_, gui_, config.frequency) {
  teams_.seedInitialEggs(world_, config_.clientsPerTeam, rng_);
  registerHandshake();
  registerAiHandlers();
  registerFallbacks();
  installAiActionPipeline(server_, scheduler_, timeUnit_);
  installGuiQueryHandlers(server_, config_, world_, players_, timeUnit_);
  server_.onDisconnect([this](Session& session) {
    session.ctx().pendingActions.clear();
    session.ctx().actionInFlight = false;
    if (session.ctx().playerId != 0) {
      const int playerId = session.ctx().playerId;
      players_.remove(playerId, world_);
      session.ctx().playerId = 0;
      gui_.playerDied(playerId);
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
  scheduler_.runDue(Scheduler::Clock::now(), [this] { return !gameOver_; });
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
  gui_.eggHatched(hatched.id);
  gui_.playerConnected(player);
  session.send(std::to_string(teams_.freeSlots(teamName)));
  session.send(worldSizeLine());
  session.completeHandshake();
  hunger_.armFirstTick(session.fd(), Scheduler::Clock::now());
}

std::string GameServer::worldSizeLine() const {
  return std::to_string(config_.width) + " " + std::to_string(config_.height);
}

void GameServer::registerAiHandlers() {
  const AiHandlerContext context{
      .players = players_, .map = world_, .teams = teams_, .gui = gui_};
  installMovementHandlers(server_, context);
  installResourceHandlers(server_, context);
  installObservationHandlers(server_, context);
  installReproductionHandlers(server_, context);
  installCommunicationHandlers(server_, context);
  server_.on(protocol::ai::Incantation(),
             [this](Session& session, zappy::rpc::IMessage&) {
               executeIncantation(session);
             });
}

void GameServer::executeIncantation(Session& session) {
  const world::Player* initiator = players_.find(session.ctx().playerId);
  const int ritualX = initiator != nullptr ? initiator->x() : 0;
  const int ritualY = initiator != nullptr ? initiator->y() : 0;
  const int ritualLevel = initiator != nullptr ? initiator->level() : 0;
  const world::IncantationOutcome outcome =
      world::getIncantationOutcome(session.ctx().playerId, players_, world_);
  if (!outcome.succeeded) {
    session.send(protocol::ai::Ko().opcode());
    return;
  }
  gui_.incantationStarted(ritualX, ritualY, ritualLevel, outcome.participants);
  const std::string levelLine =
      "Current level: " + std::to_string(outcome.newLevel);
  for (int participantId : outcome.participants) {
    server_.forEachSession([participantId, &levelLine](Session& other) {
      if (other.ctx().playerId == participantId) {
        other.send(levelLine);
      }
    });
    const world::Player* participant = players_.find(participantId);
    if (participant != nullptr) {
      gui_.playerLevel(*participant);
    }
  }
  gui_.incantationEnded(ritualX, ritualY, true);
  const std::optional<std::string> winningTeam =
      world::findWinningTeam(players_);
  if (winningTeam.has_value()) {
    endGame(*winningTeam);
  }
}

void GameServer::endGame(const std::string& winningTeam) {
  gui_.endGame(winningTeam);
  gui_.serverMessage(winningTeam + " reached the victory threshold");
  gameOver_ = true;
  stop();
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
