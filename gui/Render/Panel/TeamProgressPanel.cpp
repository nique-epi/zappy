/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TeamProgressPanel — permanent scoreboard of each team's victory progress
*/

#include "Render/Panel/TeamProgressPanel.hpp"
#include <raylib.h>
#include <algorithm>
#include <cstddef>
#include <format>
#include <string>
#include <vector>
#include "Render/Panel/PanelChrome.hpp"
#include "Render/Panel/PanelConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "World/WorldQuery.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr float blockHeight =
    cfg::TEAM_PANEL_NAME_ROW_HEIGHT + cfg::TEAM_PANEL_BAR_ROW_HEIGHT;

bool isChampion(const WorldState& world, const TeamProgress& entry) {
  return entry.maxLevelCount >= PLAYERS_REQUIRED_FOR_VICTORY ||
         (world.gameOver && entry.name == world.winnerTeam);
}

float panelHeight(std::size_t teamCount) {
  if (teamCount == 0) {
    return cfg::PANEL_HEADER_HEIGHT + cfg::PANEL_ROW_HEIGHT +
           cfg::TEAM_PANEL_BOTTOM_PADDING;
  }
  const auto count = static_cast<float>(teamCount);
  return cfg::PANEL_HEADER_HEIGHT + (count * blockHeight) +
         ((count - 1.0F) * cfg::TEAM_PANEL_BLOCK_GAP) +
         cfg::TEAM_PANEL_BOTTOM_PADDING;
}

Rectangle panelRect(std::size_t teamCount) {
  const float panelX = static_cast<float>(GetScreenWidth()) -
                       cfg::TEAM_PANEL_WIDTH - cfg::TEAM_PANEL_MARGIN;
  return Rectangle{panelX, cfg::TEAM_PANEL_TOP, cfg::TEAM_PANEL_WIDTH,
                   panelHeight(teamCount)};
}

void drawHeader(const Rectangle& panel) {
  DrawText("Teams", static_cast<int>(panel.x + cfg::PANEL_PADDING),
           static_cast<int>(panel.y + cfg::PANEL_PADDING),
           cfg::PANEL_TITLE_FONT_SIZE, cfg::PANEL_TITLE_COLOR);
  const Rectangle accent{
      panel.x + cfg::PANEL_PADDING, panel.y + cfg::PANEL_ACCENT_TOP,
      cfg::TEAM_PANEL_CONTENT_WIDTH, cfg::PANEL_ACCENT_THICKNESS};
  DrawRectangleRec(accent, cfg::PANEL_ACCENT_COLOR);
}

void drawEmptyState(const Rectangle& panel) {
  DrawText("Waiting for teams...",
           static_cast<int>(panel.x + cfg::PANEL_PADDING),
           static_cast<int>(panel.y + cfg::PANEL_HEADER_HEIGHT +
                            cfg::PANEL_ROW_TEXT_TOP),
           cfg::PANEL_ROW_FONT_SIZE, cfg::PANEL_DIM_COLOR);
}

void drawRightAligned(const std::string& text, float rightEdge, float topY,
                      Color color) {
  const auto width =
      static_cast<float>(MeasureText(text.c_str(), cfg::PANEL_ROW_FONT_SIZE));
  DrawText(text.c_str(), static_cast<int>(rightEdge - width),
           static_cast<int>(topY), cfg::PANEL_ROW_FONT_SIZE, color);
}

std::string elideToWidth(const std::string& text, int fontSize,
                         float maxWidth) {
  if (static_cast<float>(MeasureText(text.c_str(), fontSize)) <= maxWidth) {
    return text;
  }
  std::string elided = text;
  while (!elided.empty() &&
         static_cast<float>(MeasureText((elided + "...").c_str(), fontSize)) >
             maxWidth) {
    elided.pop_back();
  }
  return elided + "...";
}

void drawNameRow(const WorldState& world, const Rectangle& panel, float rowY,
                 const TeamProgress& entry, bool champion) {
  const float contentX = panel.x + cfg::PANEL_PADDING;
  const Rectangle swatch{contentX, rowY + cfg::PANEL_SWATCH_TOP,
                         cfg::PANEL_SWATCH_SIZE, cfg::PANEL_SWATCH_SIZE};
  DrawRectangleRounded(swatch, cfg::PANEL_SWATCH_ROUNDNESS,
                       cfg::PANEL_SWATCH_SEGMENTS,
                       teamColor(world, entry.name));
  DrawRectangleRoundedLinesEx(
      swatch, cfg::PANEL_SWATCH_ROUNDNESS, cfg::PANEL_SWATCH_SEGMENTS,
      cfg::PANEL_BORDER_THICKNESS, cfg::PANEL_SWATCH_BORDER_COLOR);

  const std::string aliveText = std::format("{} alive", entry.aliveCount);
  const auto aliveWidth = static_cast<float>(
      MeasureText(aliveText.c_str(), cfg::PANEL_ROW_FONT_SIZE));
  const float rightEdge = panel.x + panel.width - cfg::PANEL_PADDING;
  const float textY = rowY + cfg::PANEL_ROW_TEXT_TOP;

  const float nameX = contentX + cfg::PANEL_SWATCH_SIZE + cfg::PANEL_SWATCH_GAP;
  const float nameMaxWidth =
      rightEdge - aliveWidth - cfg::TEAM_PANEL_NAME_ALIVE_GAP - nameX;
  const std::string name =
      elideToWidth(entry.name, cfg::PANEL_ROW_FONT_SIZE, nameMaxWidth);
  const Color nameColor =
      champion ? cfg::TEAM_PANEL_VICTORY_COLOR : cfg::PANEL_TITLE_COLOR;
  DrawText(name.c_str(), static_cast<int>(nameX), static_cast<int>(textY),
           cfg::PANEL_ROW_FONT_SIZE, nameColor);

  drawRightAligned(aliveText, rightEdge, textY, cfg::PANEL_LABEL_COLOR);
}

void drawProgressBar(const Rectangle& panel, float rowY,
                     const TeamProgress& entry, bool champion) {
  const float contentX = panel.x + cfg::PANEL_PADDING;
  const Color textColor =
      champion ? cfg::TEAM_PANEL_VICTORY_COLOR : cfg::PANEL_VALUE_COLOR;
  const std::string goalText = std::format("Lv8 {}/{}", entry.maxLevelCount,
                                           PLAYERS_REQUIRED_FOR_VICTORY);
  DrawText(goalText.c_str(), static_cast<int>(contentX),
           static_cast<int>(rowY + cfg::PANEL_ROW_TEXT_TOP),
           cfg::PANEL_ROW_FONT_SIZE, textColor);

  const float barX = contentX + cfg::TEAM_PANEL_BAR_LABEL_WIDTH;
  const float barWidth = (panel.x + panel.width - cfg::PANEL_PADDING) - barX;
  const float barY =
      rowY +
      ((cfg::TEAM_PANEL_BAR_ROW_HEIGHT - cfg::TEAM_PANEL_BAR_HEIGHT) / 2.0F);
  const Rectangle track{barX, barY, barWidth, cfg::TEAM_PANEL_BAR_HEIGHT};
  DrawRectangleRounded(track, cfg::TEAM_PANEL_BAR_ROUNDNESS,
                       cfg::TEAM_PANEL_BAR_SEGMENTS,
                       cfg::TEAM_PANEL_BAR_TRACK_COLOR);

  const int reached =
      std::min(entry.maxLevelCount, PLAYERS_REQUIRED_FOR_VICTORY);
  const float ratio = static_cast<float>(reached) /
                      static_cast<float>(PLAYERS_REQUIRED_FOR_VICTORY);
  if (ratio > 0.0F) {
    const Rectangle fill{barX, barY, barWidth * ratio,
                         cfg::TEAM_PANEL_BAR_HEIGHT};
    const Color fillColor = champion ? cfg::TEAM_PANEL_VICTORY_COLOR
                                     : cfg::TEAM_PANEL_BAR_FILL_COLOR;
    DrawRectangleRounded(fill, cfg::TEAM_PANEL_BAR_ROUNDNESS,
                         cfg::TEAM_PANEL_BAR_SEGMENTS, fillColor);
  }
  DrawRectangleRoundedLinesEx(
      track, cfg::TEAM_PANEL_BAR_ROUNDNESS, cfg::TEAM_PANEL_BAR_SEGMENTS,
      cfg::PANEL_BORDER_THICKNESS, cfg::PANEL_BORDER_COLOR);
}

void drawChampionHighlight(const Rectangle& panel, float blockY) {
  const Rectangle highlight{
      panel.x + cfg::TEAM_PANEL_VICTORY_INSET,
      blockY - cfg::TEAM_PANEL_VICTORY_INSET,
      panel.width - (2.0F * cfg::TEAM_PANEL_VICTORY_INSET),
      blockHeight + cfg::TEAM_PANEL_VICTORY_INSET};
  DrawRectangleRoundedLinesEx(
      highlight, cfg::PANEL_ROUNDNESS, cfg::PANEL_SEGMENTS,
      cfg::TEAM_PANEL_VICTORY_BORDER_THICKNESS, cfg::TEAM_PANEL_VICTORY_COLOR);
}

void drawTeamBlock(const WorldState& world, const Rectangle& panel,
                   float blockY, const TeamProgress& entry) {
  const bool champion = isChampion(world, entry);
  if (champion) {
    drawChampionHighlight(panel, blockY);
  }
  drawNameRow(world, panel, blockY, entry, champion);
  drawProgressBar(panel, blockY + cfg::TEAM_PANEL_NAME_ROW_HEIGHT, entry,
                  champion);
}

}  // namespace

void TeamProgressPanel::draw(const WorldState& world) {
  const std::vector<TeamProgress> progress = computeTeamProgress(world);
  const Rectangle panel = panelRect(progress.size());

  drawPanelBackground(panel);
  drawHeader(panel);

  if (progress.empty()) {
    drawEmptyState(panel);
    return;
  }

  float blockY = panel.y + cfg::PANEL_HEADER_HEIGHT;
  for (const TeamProgress& entry : progress) {
    drawTeamBlock(world, panel, blockY, entry);
    blockY += blockHeight + cfg::TEAM_PANEL_BLOCK_GAP;
  }
}

}  // namespace zappy::gui
