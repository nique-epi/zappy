/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerPanel — fixed detail panel for the selected player
*/

#include "Render/Panel/PlayerPanel.hpp"
#include <raylib.h>
#include <format>
#include <string>
#include "Render/Panel/PanelChrome.hpp"
#include "Render/Panel/PanelConfig.hpp"
#include "Render/Panel/ResourceList.hpp"
#include "Render/RenderUtils.hpp"
#include "World/WorldQuery.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

const char* orientationName(Orientation orientation) {
  switch (orientation) {
    case Orientation::North:
      return "North";
    case Orientation::East:
      return "East";
    case Orientation::South:
      return "South";
    case Orientation::West:
      return "West";
  }
  return "North";
}

Rectangle panelRect() {
  const float height =
      cfg::PANEL_HEADER_HEIGHT +
      (cfg::PLAYER_PANEL_INFO_ROWS * cfg::PANEL_ROW_HEIGHT) +
      cfg::PLAYER_PANEL_SECTION_GAP + cfg::PANEL_ROW_HEIGHT +
      (static_cast<float>(RESOURCE_COUNT) * cfg::PANEL_ROW_HEIGHT) +
      cfg::PLAYER_PANEL_BOTTOM_PADDING;
  return Rectangle{cfg::PLAYER_PANEL_X, cfg::PLAYER_PANEL_Y,
                   cfg::PLAYER_PANEL_WIDTH, height};
}

Rectangle closeButtonRect(const Rectangle& panel) {
  return Rectangle{
      panel.x + panel.width - cfg::PANEL_PADDING - cfg::PLAYER_PANEL_CLOSE_SIZE,
      panel.y + cfg::PLAYER_PANEL_CLOSE_TOP, cfg::PLAYER_PANEL_CLOSE_SIZE,
      cfg::PLAYER_PANEL_CLOSE_SIZE};
}

void drawCloseButton(const Rectangle& button, bool hovered) {
  const Color color = hovered ? cfg::PLAYER_PANEL_CLOSE_HOVER_COLOR
                              : cfg::PLAYER_PANEL_CLOSE_COLOR;
  DrawLineEx(Vector2{button.x, button.y},
             Vector2{button.x + button.width, button.y + button.height},
             cfg::PLAYER_PANEL_CLOSE_THICKNESS, color);
  DrawLineEx(Vector2{button.x + button.width, button.y},
             Vector2{button.x, button.y + button.height},
             cfg::PLAYER_PANEL_CLOSE_THICKNESS, color);
}

void drawHeader(const Rectangle& panel, const Player& player, bool closeHover) {
  const std::string title = std::format("Player #{}", player.id);
  DrawText(title.c_str(), static_cast<int>(panel.x + cfg::PANEL_PADDING),
           static_cast<int>(panel.y + cfg::PANEL_PADDING),
           cfg::PANEL_TITLE_FONT_SIZE, cfg::PANEL_TITLE_COLOR);
  drawCloseButton(closeButtonRect(panel), closeHover);
  const Rectangle accent{
      panel.x + cfg::PANEL_PADDING, panel.y + cfg::PANEL_ACCENT_TOP,
      cfg::PLAYER_PANEL_CONTENT_WIDTH, cfg::PANEL_ACCENT_THICKNESS};
  DrawRectangleRec(accent, cfg::PANEL_ACCENT_COLOR);
}

void drawInfoRow(const Rectangle& panel, float rowY, const char* label,
                 const std::string& value, Color valueColor) {
  const int textY = static_cast<int>(rowY + cfg::PANEL_ROW_TEXT_TOP);
  DrawText(label, static_cast<int>(panel.x + cfg::PANEL_PADDING), textY,
           cfg::PANEL_ROW_FONT_SIZE, cfg::PANEL_LABEL_COLOR);
  const int valueWidth = MeasureText(value.c_str(), cfg::PANEL_ROW_FONT_SIZE);
  const float valueX = panel.x + panel.width - cfg::PANEL_PADDING -
                       static_cast<float>(valueWidth);
  DrawText(value.c_str(), static_cast<int>(valueX), textY,
           cfg::PANEL_ROW_FONT_SIZE, valueColor);
}

}  // namespace

bool PlayerPanel::draw(const WorldState& world, int playerId) {
  const Player* player = findPlayerById(world, playerId);
  if (player == nullptr || !player->alive) {
    return false;
  }
  const Rectangle panel = panelRect();
  const bool closeHover =
      CheckCollisionPointRec(GetMousePosition(), closeButtonRect(panel));

  drawPanelBackground(panel);
  drawHeader(panel, *player, closeHover);

  float rowY = panel.y + cfg::PANEL_HEADER_HEIGHT;
  drawInfoRow(panel, rowY, "Team", player->teamName,
              teamColor(world, player->teamName));
  rowY += cfg::PANEL_ROW_HEIGHT;
  drawInfoRow(panel, rowY, "Level", std::to_string(player->level),
              cfg::PANEL_VALUE_COLOR);
  rowY += cfg::PANEL_ROW_HEIGHT;
  drawInfoRow(panel, rowY, "Facing", orientationName(player->orientation),
              cfg::PANEL_VALUE_COLOR);
  rowY += cfg::PANEL_ROW_HEIGHT + cfg::PLAYER_PANEL_SECTION_GAP;

  DrawText("Inventory", static_cast<int>(panel.x + cfg::PANEL_PADDING),
           static_cast<int>(rowY + cfg::PANEL_ROW_TEXT_TOP),
           cfg::PANEL_ROW_FONT_SIZE, cfg::PANEL_DIM_COLOR);
  rowY += cfg::PANEL_ROW_HEIGHT;

  drawResourceList(Vector2{panel.x + cfg::PANEL_PADDING, rowY},
                   cfg::PLAYER_PANEL_CONTENT_WIDTH, player->inventory);

  return closeHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool PlayerPanel::contains(Vector2 point) {
  return CheckCollisionPointRec(point, panelRect());
}

}  // namespace zappy::gui
