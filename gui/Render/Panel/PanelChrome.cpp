/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PanelChrome — shared rounded background / shadow / border for GUI panels
*/

#include "Render/Panel/PanelChrome.hpp"
#include <raylib.h>
#include "Render/Panel/PanelConfig.hpp"

namespace zappy::gui {

namespace cfg = config;

void drawPanelBackground(const Rectangle& panel) {
  const Rectangle shadow{panel.x + cfg::PANEL_SHADOW_OFFSET,
                         panel.y + cfg::PANEL_SHADOW_OFFSET, panel.width,
                         panel.height};
  DrawRectangleRounded(shadow, cfg::PANEL_ROUNDNESS, cfg::PANEL_SEGMENTS,
                       cfg::PANEL_SHADOW_COLOR);
  DrawRectangleRounded(panel, cfg::PANEL_ROUNDNESS, cfg::PANEL_SEGMENTS,
                       cfg::PANEL_BG_COLOR);
  DrawRectangleRoundedLinesEx(panel, cfg::PANEL_ROUNDNESS, cfg::PANEL_SEGMENTS,
                              cfg::PANEL_BORDER_THICKNESS,
                              cfg::PANEL_BORDER_COLOR);
}

}  // namespace zappy::gui
