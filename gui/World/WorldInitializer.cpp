/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldInitializer — sequential msz/mct/tna startup sequence
*/

#include "World/WorldInitializer.hpp"
#include <chrono>
#include <sstream>
#include <string>
#include "Exceptions/WorldInitException.hpp"
#include "Protocol/GuiProtocol.hpp"

namespace zappy::gui {

namespace {
std::string firstToken(const std::string& line) {
  std::istringstream stream(line);
  std::string token;
  stream >> token;
  return token;
}
}  // namespace

WorldInitializer::WorldInitializer(INetworkClient& network,
                                   MessageParser& parser, WorldState& world,
                                   int timeoutMs)
    : network_(network),
      parser_(parser),
      world_(world),
      deadline_(std::chrono::steady_clock::now() +
                std::chrono::milliseconds(timeoutMs)) {
  network_.setResponseHandler(
      [this](const std::string& line) { onServerLine(line); });
  network_.sendLine(protocol::RequestMapSize().opcode());
}

WorldInitializer::~WorldInitializer() { network_.setResponseHandler(nullptr); }

bool WorldInitializer::isDone() const { return phase_ == WorldInitPhase::Done; }

void WorldInitializer::checkTimeout() const {
  if (isDone()) {
    return;
  }
  if (std::chrono::steady_clock::now() >= deadline_) {
    throw WorldInitTimeoutException();
  }
}

void WorldInitializer::onPollRoundComplete() {
  if (phase_ != WorldInitPhase::WaitingTeamNames) {
    return;
  }
  if (receivedAnyTeamName_ && !receivedTeamNameThisRound_) {
    phase_ = WorldInitPhase::Done;
  }
  receivedTeamNameThisRound_ = false;
}

void WorldInitializer::onServerLine(const std::string& line) {
  parser_.parseLine(line);

  const std::string token = firstToken(line);

  switch (phase_) {
    case WorldInitPhase::WaitingMapSize:
      handleMapSizeToken(token);
      break;
    case WorldInitPhase::WaitingMapContent:
      handleTileContentToken(token);
      break;
    case WorldInitPhase::WaitingTeamNames:
      handleTeamNameToken(token);
      break;
    case WorldInitPhase::Done:
      break;
  }
}

void WorldInitializer::handleMapSizeToken(const std::string& token) {
  if (token != protocol::RequestMapSize().opcode()) {
    return;
  }
  expectedTileCount_ = world_.width * world_.height;
  if (expectedTileCount_ == 0) {
    phase_ = WorldInitPhase::WaitingTeamNames;
    network_.sendLine(protocol::RequestTeamNames().opcode());
  } else {
    phase_ = WorldInitPhase::WaitingMapContent;
    network_.sendLine(protocol::RequestMapContent().opcode());
  }
}

void WorldInitializer::handleTileContentToken(const std::string& token) {
  if (token != protocol::TileContent().opcode()) {
    return;
  }
  receivedTileCount_++;
  if (receivedTileCount_ >= expectedTileCount_) {
    phase_ = WorldInitPhase::WaitingTeamNames;
    network_.sendLine(protocol::RequestTeamNames().opcode());
  }
}

void WorldInitializer::handleTeamNameToken(const std::string& token) {
  if (token != protocol::RequestTeamNames().opcode()) {
    return;
  }
  receivedAnyTeamName_ = true;
  receivedTeamNameThisRound_ = true;
}

}  // namespace zappy::gui
