/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for PlayerSelection state transitions
*/

#include <gtest/gtest.h>
#include <optional>
#include "Render/Entity/PlayerSelection.hpp"
#include "World/WorldState.hpp"

using zappy::gui::Player;
using zappy::gui::PlayerSelection;
using zappy::gui::WorldState;

namespace {

constexpr int selectedPlayerId = 7;
constexpr int otherPlayerId = 9;

WorldState worldWithPlayer(int playerId, bool alive) {
  WorldState world;
  world.players.push_back(Player{.id = playerId, .alive = alive});
  return world;
}

}  // namespace

// Given no current selection
// When a hovered player is clicked outside the panel
// Then it becomes selected and is reported as the newly selected id
TEST(PlayerSelection, ClickSelectsHoveredPlayer) {
  PlayerSelection selection;

  const std::optional<int> changed =
      selection.click(std::optional<int>(selectedPlayerId), false);

  EXPECT_EQ(changed, std::optional<int>(selectedPlayerId));
  EXPECT_EQ(selection.selectedId(), std::optional<int>(selectedPlayerId));
}

// Given a player already selected
// When the same player is clicked again
// Then no change is reported so the caller skips a redundant refresh
TEST(PlayerSelection, ReclickingSelectedPlayerReportsNoChange) {
  PlayerSelection selection;
  selection.click(std::optional<int>(selectedPlayerId), false);

  const std::optional<int> changed =
      selection.click(std::optional<int>(selectedPlayerId), false);

  EXPECT_EQ(changed, std::nullopt);
  EXPECT_EQ(selection.selectedId(), std::optional<int>(selectedPlayerId));
}

// Given a different player is hovered
// When it is clicked
// Then the selection switches and the new id is reported
TEST(PlayerSelection, ClickSwitchesToAnotherPlayer) {
  PlayerSelection selection;
  selection.click(std::optional<int>(selectedPlayerId), false);

  const std::optional<int> changed =
      selection.click(std::optional<int>(otherPlayerId), false);

  EXPECT_EQ(changed, std::optional<int>(otherPlayerId));
  EXPECT_EQ(selection.selectedId(), std::optional<int>(otherPlayerId));
}

// Given a current selection
// When clicking empty space outside the panel
// Then the selection is cleared
TEST(PlayerSelection, ClickOutsideClearsSelection) {
  PlayerSelection selection;
  selection.click(std::optional<int>(selectedPlayerId), false);

  const std::optional<int> changed = selection.click(std::nullopt, false);

  EXPECT_EQ(changed, std::nullopt);
  EXPECT_FALSE(selection.selectedId().has_value());
}

// Given a current selection
// When clicking inside the panel with no player hovered
// Then the selection is kept so the panel can handle its own controls
TEST(PlayerSelection, ClickInsidePanelKeepsSelection) {
  PlayerSelection selection;
  selection.click(std::optional<int>(selectedPlayerId), false);

  const std::optional<int> changed = selection.click(std::nullopt, true);

  EXPECT_EQ(changed, std::nullopt);
  EXPECT_EQ(selection.selectedId(), std::optional<int>(selectedPlayerId));
}

// Given the selected player dies
// When syncing with the world
// Then the selection is cleared so the panel closes
TEST(PlayerSelection, SyncClearsSelectionWhenPlayerDies) {
  PlayerSelection selection;
  selection.click(std::optional<int>(selectedPlayerId), false);

  selection.syncWithWorld(worldWithPlayer(selectedPlayerId, false));

  EXPECT_FALSE(selection.selectedId().has_value());
}

// Given the selected player is absent from the world
// When syncing with the world
// Then the selection is cleared
TEST(PlayerSelection, SyncClearsSelectionWhenPlayerGone) {
  PlayerSelection selection;
  selection.click(std::optional<int>(selectedPlayerId), false);

  selection.syncWithWorld(WorldState{});

  EXPECT_FALSE(selection.selectedId().has_value());
}

// Given the selected player is still alive
// When syncing with the world
// Then the selection is kept
TEST(PlayerSelection, SyncKeepsSelectionWhenPlayerAlive) {
  PlayerSelection selection;
  selection.click(std::optional<int>(selectedPlayerId), false);

  selection.syncWithWorld(worldWithPlayer(selectedPlayerId, true));

  EXPECT_EQ(selection.selectedId(), std::optional<int>(selectedPlayerId));
}
