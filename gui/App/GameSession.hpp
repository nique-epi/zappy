/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GameSession — drives one connected spectator session (input/update/render)
*/

#pragma once

#include <optional>
#include "Bindings/KeyBindings.hpp"
#include "GuiConfig.hpp"
#include "Network/CommandSender.hpp"
#include "Network/Parsing/MessageParser.hpp"
#include "Render/Entity/PlayerSelection.hpp"
#include "Render/SpeedControl.hpp"
#include "Render/Stats/Panel/StatsPanel.hpp"
#include "Render/WorldCamera.hpp"
#include "Render/WorldRenderer.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

class NetworkManager;

/**
 * @brief Runs a single connected spectator session until quit or back-to-menu.
 *
 * Owns the world, the parser fed by the network, the command sender, the
 * camera and the entity renderers. The render loop follows the classic
 * game-loop structure: @ref update, @ref processInput, @ref render.
 */
class GameSession {
 public:
  GameSession(const GuiConfig& config, NetworkManager& network,
              const KeyBindings& bindings);
  ~GameSession() = default;

  GameSession(const GameSession&) = delete;
  GameSession& operator=(const GameSession&) = delete;
  GameSession(GameSession&&) = delete;
  GameSession& operator=(GameSession&&) = delete;

  /**
   * @brief Run the session loop.
   *
   * @returns true if the user asked to return to the menu, false to quit.
   */
  bool run();

 private:
  bool connectAndInitialize();
  void update();
  void processInput();
  void render();

  const GuiConfig& config_;
  NetworkManager& network_;
  const KeyBindings& bindings_;
  WorldState world_;
  MessageParser parser_;
  CommandSender sender_;
  SpeedControl speedControl_;
  StatsPanel statsPanel_;
  PlayerSelection selection_;
  std::optional<WorldCamera> camera_;
  std::optional<WorldRenderer> worldRenderer_;
  std::optional<int> hoveredPlayer_;
  bool returnToMenu_{false};
};

}  // namespace zappy::gui
