/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerSelection — tracks the spectator-selected player across frames
*/

#include "Render/Entity/PlayerSelection.hpp"
#include <optional>
#include "Render/RenderUtils.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

std::optional<int> PlayerSelection::click(std::optional<int> hoveredId,
                                          bool clickInsidePanel) {
  if (clickInsidePanel) {
    return std::nullopt;
  }
  if (hoveredId.has_value()) {
    selectedId_ = hoveredId;
    return hoveredId;
  }
  selectedId_.reset();
  return std::nullopt;
}

void PlayerSelection::syncWithWorld(const WorldState& world) {
  if (!selectedId_.has_value()) {
    return;
  }
  const Player* player = findPlayerById(world, *selectedId_);
  if (player == nullptr || !player->alive) {
    selectedId_.reset();
  }
}

void PlayerSelection::close() { selectedId_.reset(); }

std::optional<int> PlayerSelection::selectedId() const { return selectedId_; }

}  // namespace zappy::gui
