/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** HudPanel — always-on overlay with the global session / world summary
*/

#include "Render/Panel/HudPanel.hpp"
#include <raylib.h>
#include <format>
#include <string>
#include "GuiConfig.hpp"
#include "Render/WindowConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

void HudPanel::draw(const GuiConfig& config, const WorldState& world) {
  const std::string summary =
      std::format("{}:{}  |  Map: {}x{}  |  Players: {}  |  Teams: {}",
                  config.hostname, config.port, world.width, world.height,
                  world.players.size(), world.teams.size());

  DrawText(cfg::WINDOW_TITLE, cfg::MARGIN_X, cfg::TITLE_Y, cfg::TITLE_FONT_SIZE,
           WHITE);
  DrawText(summary.c_str(), cfg::MARGIN_X, cfg::HUD_Y, cfg::HUD_FONT_SIZE,
           LIGHTGRAY);
}

}  // namespace zappy::gui
