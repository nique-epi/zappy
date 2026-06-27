/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** StatsPanel — live statistics panel with per-team progression charts
*/

#include "Render/Stats/Panel/StatsPanel.hpp"
#include <raylib.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <string>
#include "Render/Panel/PanelConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "Render/Stats/Chart/TeamProgressBar.hpp"
#include "Render/Stats/StatsConfig.hpp"
#include "Render/raygui.h"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

namespace {

constexpr std::array<const char*, RESOURCE_COUNT> resourceAbbreviations = {
    "food", "lnmt", "drau", "sibr", "mend", "phrs", "thys"};

constexpr std::array<Color, RESOURCE_COUNT> resourceColors = {{
    cfg::RESOURCE_FOOD_COLOR,
    cfg::RESOURCE_LINEMATE_COLOR,
    cfg::RESOURCE_DERAUMERE_COLOR,
    cfg::RESOURCE_SIBUR_COLOR,
    cfg::RESOURCE_MENDIANE_COLOR,
    cfg::RESOURCE_PHIRAS_COLOR,
    cfg::RESOURCE_THYSTAME_COLOR,
}};

constexpr std::array<float, 4> iconBarRatios = {0.30F, 0.55F, 0.75F, 1.00F};

constexpr std::array<Color, 4> iconBarColors = {{
    cfg::STATS_ICON_BAR1,
    cfg::STATS_ICON_BAR2,
    cfg::STATS_ICON_BAR3,
    cfg::STATS_ICON_BAR4,
}};

int countAlivePlayersInTeam(const WorldState& world,
                            const std::string& teamName) {
  int count = 0;
  for (const auto& player : world.players) {
    if (player.alive && player.teamName == teamName) {
      ++count;
    }
  }
  return count;
}

int maxLevelInTeam(const WorldState& world, const std::string& teamName) {
  int maxLevel = 0;
  for (const auto& player : world.players) {
    if (player.alive && player.teamName == teamName) {
      maxLevel = std::max(maxLevel, player.level);
    }
  }
  return maxLevel;
}

int countAlivePlayers(const WorldState& world) {
  int count = 0;
  for (const auto& player : world.players) {
    if (player.alive) {
      ++count;
    }
  }
  return count;
}

std::array<int, RESOURCE_COUNT> totalResourcesOnMap(const WorldState& world) {
  std::array<int, RESOURCE_COUNT> totals{};
  for (const auto& row : world.tiles) {
    for (const auto& tile : row) {
      for (std::size_t i = 0; i < RESOURCE_COUNT; ++i) {
        totals[i] += tile.resources[i];
      }
    }
  }
  return totals;
}

void drawResourceGrid(float originX, float originY,
                      const std::array<int, RESOURCE_COUNT>& resources) {
  const float columnWidth = cfg::STATS_PANEL_CONTENT_W / 2.0F;

  for (std::size_t i = 0; i < RESOURCE_COUNT; ++i) {
    const float columnX = originX + (static_cast<float>(i % 2) * columnWidth);
    const std::size_t row = i / 2U;
    const float rowY = originY + (static_cast<float>(row) * cfg::STATS_ROW_H);

    DrawRectangleRounded(
        {columnX,
         rowY + ((cfg::STATS_ROW_H - cfg::STATS_RES_SWATCH_SIZE) / 2.0F),
         cfg::STATS_RES_SWATCH_SIZE, cfg::STATS_RES_SWATCH_SIZE},
        0.3F, 4, resourceColors[i]);

    const std::string text =
        std::format("{}: {}", resourceAbbreviations[i], resources[i]);
    DrawText(text.c_str(),
             static_cast<int>(columnX + cfg::STATS_RES_SWATCH_SIZE +
                              cfg::STATS_RES_SWATCH_GAP),
             static_cast<int>(
                 rowY +
                 ((cfg::STATS_ROW_H - static_cast<float>(cfg::STATS_ROW_FONT)) /
                  2.0F)),
             cfg::STATS_ROW_FONT, cfg::STATS_TEXT_LABEL);
  }
}

}  // namespace

void StatsPanel::draw(const WorldState& world) {
  drawIconButton();
  if (isOpen_) {
    lastPanelRect_ = panelRect(world);
    drawPanel(world);
  }
}

bool StatsPanel::contains(Vector2 point) const {
  if (CheckCollisionPointRec(point, buttonRect())) {
    return true;
  }
  return isOpen_ && CheckCollisionPointRec(point, lastPanelRect_);
}

Rectangle StatsPanel::buttonRect() {
  const float size = cfg::STATS_BTN_SIZE;
  return {static_cast<float>(GetScreenWidth()) - size - cfg::STATS_BTN_MARGIN,
          static_cast<float>(GetScreenHeight()) - size - cfg::STATS_BTN_MARGIN,
          size, size};
}

float StatsPanel::computePanelHeight(const WorldState& world) {
  const float perTeam = cfg::STATS_ROW_H + cfg::STATS_ROW_H +
                        cfg::STATS_CHART_H + cfg::STATS_CHART_LABEL_GAP +
                        cfg::STATS_CHART_LABELS_H + cfg::STATS_TEAM_GAP;

  float height = cfg::STATS_GUI_HEADER_H;
  height += cfg::STATS_SECTION_H;
  height += static_cast<float>(world.teams.size()) * perTeam;
  height += cfg::STATS_SECTION_GAP + cfg::STATS_SECTION_H;
  height += cfg::STATS_ROW_H;
  height += 4.0F * cfg::STATS_ROW_H;
  height += cfg::STATS_PANEL_PADDING;

  const float available = static_cast<float>(GetScreenHeight()) -
                          cfg::STATS_PANEL_Y_START - cfg::STATS_BTN_MARGIN;
  return std::min(height, available);
}

Rectangle StatsPanel::panelRect(const WorldState& world) {
  return {static_cast<float>(GetScreenWidth()) - cfg::STATS_PANEL_WIDTH -
              cfg::STATS_PANEL_MARGIN,
          cfg::STATS_PANEL_Y_START, cfg::STATS_PANEL_WIDTH,
          computePanelHeight(world)};
}

void StatsPanel::drawIconButton() {
  const Rectangle logical = buttonRect();
  const Vector2 mouse = GetMousePosition();
  const bool hovered = CheckCollisionPointRec(mouse, logical);
  const bool held = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

  float scale = 1.0F;
  if (held) {
    scale = cfg::STATS_ICON_HELD_SCALE;
  } else if (hovered && !isOpen_) {
    scale = cfg::STATS_ICON_HOVER_SCALE;
  }

  const float centerX = logical.x + (logical.width / 2.0F);
  const float centerY = logical.y + (logical.height / 2.0F);
  const float halfW = logical.width * scale / 2.0F;
  const float halfH = logical.height * scale / 2.0F;
  const Rectangle drawRect{centerX - halfW, centerY - halfH, halfW * 2.0F,
                           halfH * 2.0F};

  renderIconButton(drawRect, centerX, logical.y, held, hovered);

  if (hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    isOpen_ = !isOpen_;
  }
}

void StatsPanel::renderIconButton(const Rectangle& drawRect, float centerX,
                                  float logicalY, bool held,
                                  bool hovered) const {
  Color background = cfg::STATS_BTN_COLOR;
  if (isOpen_) {
    background = cfg::STATS_BTN_ACTIVE;
  } else if (held) {
    background = cfg::STATS_BTN_HELD;
  } else if (hovered) {
    background = cfg::STATS_BTN_HOVER;
  }

  DrawRectangleRounded(drawRect, cfg::STATS_BTN_ROUNDNESS,
                       cfg::STATS_BTN_SEGMENTS, background);
  DrawRectangleRoundedLinesEx(
      drawRect, cfg::STATS_BTN_ROUNDNESS, cfg::STATS_BTN_SEGMENTS,
      cfg::STATS_BTN_BORDER_WIDTH, cfg::STATS_BTN_BORDER);

  drawChartIcon(drawRect);

  if (hovered) {
    const char* tooltip = "Statistics";
    const int tooltipW = MeasureText(tooltip, cfg::STATS_TOOLTIP_FONT_SIZE);
    DrawText(tooltip,
             static_cast<int>(centerX - (static_cast<float>(tooltipW) / 2.0F)),
             static_cast<int>(logicalY - cfg::STATS_TOOLTIP_GAP -
                              static_cast<float>(cfg::STATS_TOOLTIP_FONT_SIZE)),
             cfg::STATS_TOOLTIP_FONT_SIZE, cfg::STATS_TOOLTIP_COLOR);
  }
}

void StatsPanel::drawChartIcon(const Rectangle& rect) {
  const float padding = rect.width * 0.16F;
  const float innerX = rect.x + padding;
  const float innerY = rect.y + padding;
  const float innerW = rect.width - (2.0F * padding);
  const float innerH = rect.height - (2.0F * padding);

  constexpr std::size_t barCount = 4;
  const float gapW = innerW * 0.08F;
  const float barW = (innerW - (gapW * static_cast<float>(barCount - 1))) /
                     static_cast<float>(barCount);

  for (std::size_t i = 0; i < barCount; ++i) {
    const float barH = innerH * iconBarRatios[i];
    const float barX = innerX + (static_cast<float>(i) * (barW + gapW));
    const float barY = innerY + innerH - barH;
    DrawRectangleRec({barX, barY, barW, barH}, iconBarColors[i]);
  }
}

void StatsPanel::drawPanel(const WorldState& world) {
  const Rectangle& panel = lastPanelRect_;

  if (GuiWindowBox(panel, "Statistics") != 0) {
    isOpen_ = false;
    return;
  }

  float cursor = panel.y + cfg::STATS_GUI_HEADER_H + 4.0F;

  BeginScissorMode(static_cast<int>(panel.x),
                   static_cast<int>(panel.y + cfg::STATS_GUI_HEADER_H),
                   static_cast<int>(panel.width),
                   static_cast<int>(panel.height - cfg::STATS_GUI_HEADER_H));

  const float contentX = panel.x + cfg::STATS_PANEL_PADDING;

  drawSectionHeader(contentX, cursor, "Teams");
  for (const auto& teamName : world.teams) {
    drawTeamBlock(panel, cursor, world, teamName);
  }
  drawGlobalBlock(panel, cursor, world);

  EndScissorMode();
}

void StatsPanel::drawSectionHeader(float contentX, float& cursor,
                                   const char* title) {
  DrawText(title, static_cast<int>(contentX), static_cast<int>(cursor),
           cfg::STATS_SECTION_FONT, cfg::STATS_TEXT_SECTION);
  cursor += cfg::STATS_SECTION_H;
}

void StatsPanel::drawTeamBlock(const Rectangle& panel, float& cursor,
                               const WorldState& world,
                               const std::string& teamName) {
  const Color color = teamColor(world, teamName);
  const float contentX = panel.x + cfg::STATS_PANEL_PADDING;
  const float rowTextOffset =
      (cfg::STATS_ROW_H - static_cast<float>(cfg::STATS_ROW_FONT)) / 2.0F;

  DrawRectangleRounded({contentX, cursor + cfg::STATS_SWATCH_TOP,
                        cfg::STATS_SWATCH_SIZE, cfg::STATS_SWATCH_SIZE},
                       cfg::STATS_SWATCH_ROUNDNESS, cfg::STATS_SWATCH_SEGMENTS,
                       color);

  DrawText(teamName.c_str(),
           static_cast<int>(contentX + cfg::STATS_SWATCH_SIZE +
                            cfg::STATS_SWATCH_GAP),
           static_cast<int>(cursor + rowTextOffset), cfg::STATS_ROW_FONT,
           cfg::STATS_TEXT_VALUE);
  cursor += cfg::STATS_ROW_H;

  const int alive = countAlivePlayersInTeam(world, teamName);
  const int maxLevel = maxLevelInTeam(world, teamName);
  const std::string statsText =
      std::format("Alive: {}   Max lvl: {}", alive, maxLevel);
  DrawText(statsText.c_str(), static_cast<int>(contentX),
           static_cast<int>(cursor + rowTextOffset), cfg::STATS_ROW_FONT,
           cfg::STATS_TEXT_LABEL);
  cursor += cfg::STATS_ROW_H;

  drawTeamProgressBar(Vector2{contentX, cursor}, cfg::STATS_PANEL_CONTENT_W,
                      world, teamName, color);
  cursor += cfg::STATS_CHART_H + cfg::STATS_CHART_LABEL_GAP +
            cfg::STATS_CHART_LABELS_H + cfg::STATS_TEAM_GAP;
}

void StatsPanel::drawGlobalBlock(const Rectangle& panel, float& cursor,
                                 const WorldState& world) {
  const float contentX = panel.x + cfg::STATS_PANEL_PADDING;
  cursor += cfg::STATS_SECTION_GAP;
  drawSectionHeader(contentX, cursor, "Global");

  const float rowTextOffset =
      (cfg::STATS_ROW_H - static_cast<float>(cfg::STATS_ROW_FONT)) / 2.0F;

  const int alive = countAlivePlayers(world);
  const int incantations = static_cast<int>(world.activeIncantations.size()) +
                           static_cast<int>(world.finishedIncantations.size());
  const std::string summaryText =
      std::format("Alive: {}   Incant.: {}", alive, incantations);
  DrawText(summaryText.c_str(), static_cast<int>(contentX),
           static_cast<int>(cursor + rowTextOffset), cfg::STATS_ROW_FONT,
           cfg::STATS_TEXT_LABEL);
  cursor += cfg::STATS_ROW_H;

  drawResourceGrid(contentX, cursor, totalResourcesOnMap(world));
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

}  // namespace zappy::gui
