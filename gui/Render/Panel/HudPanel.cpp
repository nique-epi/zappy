/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** HudPanel — always-on overlay with the global session / world summary
*/

#include "Render/Panel/HudPanel.hpp"
#include <raylib.h>
#include <algorithm>
#include <format>
#include <string>
#include "GuiConfig.hpp"
#include "Render/Panel/PanelChrome.hpp"
#include "Render/Panel/PanelConfig.hpp"
#include "Render/WindowConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr float hudTitleGap = 4.0F;
constexpr float hudMinContentWidth = 200.0F;

}  // namespace

void HudPanel::draw(const GuiConfig& config, const WorldState& world) {
  const std::string summary =
      std::format("{}:{}  |  Map: {}x{}  |  Players: {}  |  Teams: {}",
                  config.hostname, config.port, world.width, world.height,
                  world.players.size(), world.teams.size());

  const auto titleWidth =
      static_cast<float>(MeasureText(cfg::WINDOW_TITLE, cfg::TITLE_FONT_SIZE));
  const auto summaryWidth =
      static_cast<float>(MeasureText(summary.c_str(), cfg::HUD_FONT_SIZE));
  const float contentWidth =
      std::max({titleWidth, summaryWidth, hudMinContentWidth});

  const float panelHeight =
      cfg::PANEL_PADDING + static_cast<float>(cfg::TITLE_FONT_SIZE) +
      hudTitleGap + static_cast<float>(cfg::HUD_FONT_SIZE) + cfg::PANEL_PADDING;
  const Rectangle panel{
      static_cast<float>(cfg::MARGIN_X), static_cast<float>(cfg::TITLE_Y),
      contentWidth + (2.0F * cfg::PANEL_PADDING), panelHeight};

  drawPanelBackground(panel);

  const int textX = static_cast<int>(panel.x + cfg::PANEL_PADDING);
  DrawText(cfg::WINDOW_TITLE, textX,
           static_cast<int>(panel.y + cfg::PANEL_PADDING), cfg::TITLE_FONT_SIZE,
           cfg::PANEL_TITLE_COLOR);
  DrawText(
      summary.c_str(), textX,
      static_cast<int>(panel.y + cfg::PANEL_PADDING +
                       static_cast<float>(cfg::TITLE_FONT_SIZE) + hudTitleGap),
      cfg::HUD_FONT_SIZE, cfg::PANEL_LABEL_COLOR);
}

}  // namespace zappy::gui
