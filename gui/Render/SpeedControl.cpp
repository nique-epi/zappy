/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SpeedControl — Raygui overlay to tune the server time unit (sst/sgt)
*/

#include "Render/SpeedControl.hpp"
#include <raylib.h>
#include <format>
#include <string>
#include "Network/CommandSender.hpp"
#include "Render/WindowConfig.hpp"
#include "Render/raygui.h"

namespace zappy::gui {

namespace cfg = config;

namespace {

void sendFrequencyIfChanged(CommandSender& sender, int next, int current) {
  if (next != current) {
    sender.setTimeUnit(next);
  }
}

}  // namespace

SpeedControl::SpeedControl(CommandSender& sender) : sender_(sender) {}

void SpeedControl::draw(int currentFrequency) {
  const float panelX = static_cast<float>(GetScreenWidth()) -
                       cfg::SPEED_PANEL_WIDTH - cfg::SPEED_PANEL_MARGIN;
  const float contentX = panelX + cfg::SPEED_PANEL_PADDING;
  const float contentY = cfg::SPEED_PANEL_MARGIN + cfg::SPEED_HEADER_HEIGHT +
                         cfg::SPEED_PANEL_PADDING;
  const float contentWidth =
      cfg::SPEED_PANEL_WIDTH - (2.0F * cfg::SPEED_PANEL_PADDING);

  const Rectangle panel{panelX, cfg::SPEED_PANEL_MARGIN, cfg::SPEED_PANEL_WIDTH,
                        cfg::SPEED_PANEL_HEIGHT};
  const Rectangle minusButton{contentX, contentY, cfg::SPEED_BUTTON_WIDTH,
                              cfg::SPEED_ROW_HEIGHT};
  const Rectangle plusButton{contentX + contentWidth - cfg::SPEED_BUTTON_WIDTH,
                             contentY, cfg::SPEED_BUTTON_WIDTH,
                             cfg::SPEED_ROW_HEIGHT};
  const Rectangle valueLabel{contentX + cfg::SPEED_BUTTON_WIDTH, contentY,
                             contentWidth - (2.0F * cfg::SPEED_BUTTON_WIDTH),
                             cfg::SPEED_ROW_HEIGHT};
  const Rectangle hintLabel{
      contentX, contentY + cfg::SPEED_ROW_HEIGHT + cfg::SPEED_HINT_GAP,
      contentWidth, cfg::SPEED_HINT_HEIGHT};

  const std::string valueText = std::format("f = {}", currentFrequency);
  const std::string hintText =
      std::format("range {} - {}", cfg::SPEED_MIN, cfg::SPEED_MAX);

  GuiPanel(panel, "Simulation speed");

  if (GuiButton(minusButton, "-") != 0) {
    sendFrequencyIfChanged(sender_, decreaseFrequency(currentFrequency),
                           currentFrequency);
  }
  if (GuiButton(plusButton, "+") != 0) {
    sendFrequencyIfChanged(sender_, increaseFrequency(currentFrequency),
                           currentFrequency);
  }

  GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
  GuiLabel(valueLabel, valueText.c_str());
  GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
  GuiLabel(hintLabel, hintText.c_str());
}

}  // namespace zappy::gui
