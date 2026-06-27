/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GameSession — drives one connected spectator session (input/update/render)
*/

#include "App/GameSession.hpp"
#include <raylib.h>
#include <optional>
#include "Network/ConnectionBootstrap.hpp"
#include "Network/NetworkManager.hpp"
#include "Render/EndScreen/EndScreen.hpp"
#include "Render/Entity/PlayerChevron.hpp"
#include "Render/Entity/PlayerPicker.hpp"
#include "Render/Panel/HudPanel.hpp"
#include "Render/Panel/InfoPanel.hpp"
#include "Render/Panel/PlayerPanel.hpp"
#include "Render/Panel/TeamProgressPanel.hpp"
#include "Render/SkyRenderer.hpp"
#include "Render/TileGridRenderer.hpp"
#include "Render/WindowConfig.hpp"

namespace zappy::gui {

namespace cfg = config;

GameSession::GameSession(const GuiConfig& config, NetworkManager& network,
                         const KeyBindings& bindings)
    : config_(config),
      network_(network),
      bindings_(bindings),
      parser_(world_),
      sender_(network_),
      speedControl_(sender_) {}

bool GameSession::run() {
  if (!connectAndInitialize()) {
    return false;
  }
  while (!WindowShouldClose()) {
    if (IsKeyPressed(bindings_.backToMenu)) {
      returnToMenu_ = true;
      break;
    }
    update();
    processInput();
    render();
  }
  EndScreen::unloadFont();
  return returnToMenu_;
}

bool GameSession::connectAndInitialize() {
  if (!handshakeAndInitializeWorld(network_, world_, parser_)) {
    return false;
  }
  const Vector3 mapCenter{
      static_cast<float>(world_.width) * cfg::TILE_SIZE / 2.0F, 0.0F,
      static_cast<float>(world_.height) * cfg::TILE_SIZE / 2.0F};
  camera_.emplace(mapCenter);
  worldRenderer_.emplace();
  EndScreen::loadFont();
  sender_.requestTimeUnit();
  return true;
}

void GameSession::update() {
  network_.runOnce(0);
  camera_->update(GetFrameTime(), bindings_);
  selection_.syncWithWorld(world_);
  hoveredPlayer_ = PlayerPicker::nearest(world_, camera_->camera());
}

void GameSession::processInput() {
  if (IsKeyPressed(KEY_V) && !world_.gameOver) {
    world_.gameOver = true;
    world_.winnerTeam = world_.teams.empty() ? "Team1" : world_.teams.front();
  }
  if (IsKeyPressed(KEY_L) && !world_.gameOver) {
    world_.gameOver = true;
    world_.winnerTeam = world_.teams.size() > 1 ? world_.teams[1] : "Unknown";
  }
  if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    return;
  }
  const bool insidePanel = selection_.selectedId().has_value() &&
                           PlayerPanel::contains(GetMousePosition());
  const std::optional<int> picked =
      selection_.click(hoveredPlayer_, insidePanel);
  if (picked.has_value()) {
    sender_.requestPlayerInventory(*picked);
    sender_.requestPlayerLevel(*picked);
  }
}

void GameSession::render() {
  BeginDrawing();
  ClearBackground(BLACK);
  BeginMode3D(camera_->camera());
  SkyRenderer::draw(camera_->camera());
  TileGridRenderer::draw(world_);
  worldRenderer_->drawWorld(world_);
  EndMode3D();
  worldRenderer_->drawOverlay(world_, camera_->camera());
  PlayerChevron::draw(world_, camera_->camera(), hoveredPlayer_,
                      selection_.selectedId());
  HudPanel::draw(config_, world_);
  InfoPanel::draw(world_, camera_->camera());
  speedControl_.draw(world_.timeUnit);
  TeamProgressPanel::draw(world_);
  if (selection_.selectedId().has_value()) {
    if (PlayerPanel::draw(world_, *selection_.selectedId())) {
      selection_.close();
    }
  }
  if (world_.gameOver) {
    EndScreen::draw(world_, "ESC");
  }
  EndDrawing();
}

}  // namespace zappy::gui
