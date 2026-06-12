/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** CommandSender — formats and sends GUI -> server protocol commands
*/

#include "Network/CommandSender.hpp"
#include <format>
#include "Protocol/GuiProtocol.hpp"

namespace zappy::gui {

CommandSender::CommandSender(INetworkClient& network) : network_(network) {}

void CommandSender::requestMapSize() {
  network_.sendLine(protocol::RequestMapSize().opcode());
}

void CommandSender::requestMapContent() {
  network_.sendLine(protocol::RequestMapContent().opcode());
}

void CommandSender::requestTileContent(int posX, int posY) {
  network_.sendLine(std::format(
      "{} {} {}", protocol::RequestTileContent().opcode(), posX, posY));
}

void CommandSender::requestTeamNames() {
  network_.sendLine(protocol::RequestTeamNames().opcode());
}

void CommandSender::requestPlayerPosition(int playerId) {
  network_.sendLine(std::format(
      "{} #{}", protocol::RequestPlayerPosition().opcode(), playerId));
}

void CommandSender::requestPlayerLevel(int playerId) {
  network_.sendLine(
      std::format("{} #{}", protocol::RequestPlayerLevel().opcode(), playerId));
}

void CommandSender::requestPlayerInventory(int playerId) {
  network_.sendLine(std::format(
      "{} #{}", protocol::RequestPlayerInventory().opcode(), playerId));
}

void CommandSender::requestTimeUnit() {
  network_.sendLine(protocol::RequestTimeUnit().opcode());
}

void CommandSender::setTimeUnit(int timeUnit) {
  network_.sendLine(
      std::format("{} {}", protocol::SetTimeUnit().opcode(), timeUnit));
}

}  // namespace zappy::gui
