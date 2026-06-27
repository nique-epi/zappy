/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TeamResultPanel — centered team results panel for the end-of-game screen
*/

#include "Render/EndScreen/Panel/TeamResultPanel.hpp"
#include <raylib.h>
#include <cstddef>
#include <string>
#include "Render/EndScreen/EndScreenConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = endscreen::config;

namespace {

Rectangle panelRect(std::size_t teamCount) {
  const auto sw = static_cast<float>(GetScreenWidth());
  const auto sh = static_cast<float>(GetScreenHeight());
  const float height = cfg::panelHeaderHeight +
                       (static_cast<float>(teamCount) * cfg::panelRowHeight) +
                       cfg::panelBottomPad;
  return Rectangle{(sw - cfg::panelWidth) / 2.0F, sh * cfg::panelYRatio,
                   cfg::panelWidth, height};
}

void drawFrame(const Rectangle& panel) {
  const Rectangle shadow{panel.x + cfg::panelShadowOffset,
                         panel.y + cfg::panelShadowOffset, panel.width,
                         panel.height};
  DrawRectangleRounded(shadow, cfg::panelRoundness, cfg::panelSegments,
                       cfg::panelShadow);
  DrawRectangleRounded(panel, cfg::panelRoundness, cfg::panelSegments,
                       cfg::panelBg);
  DrawRectangleRoundedLinesEx(panel, cfg::panelRoundness, cfg::panelSegments,
                              cfg::panelBorderThickness, cfg::panelBorder);
}

void drawHeader(const Rectangle& panel, const Font& font) {
  const float size = static_cast<float>(cfg::headerFontSize);
  DrawTextEx(font, "Team Results",
             Vector2{panel.x + cfg::panelPadding, panel.y + cfg::panelPadding},
             size, cfg::textSpacing, cfg::headerColor);
  DrawRectangleRec(Rectangle{panel.x + cfg::panelPadding,
                             panel.y + cfg::panelHeaderHeight - 4.0F,
                             panel.width - cfg::panelPadding * 2.0F, 2.0F},
                   cfg::panelBorder);
}

void drawBadge(float rightEdge, float rowCenterY, const char* label,
               Color background, const Font& font) {
  const auto fsize = static_cast<float>(cfg::badgeFontSize);
  const Vector2 textSize = MeasureTextEx(font, label, fsize, cfg::textSpacing);
  const float badgeWidth = textSize.x + cfg::badgePaddingX * 2.0F;
  const float badgeY = rowCenterY - cfg::badgeHeight / 2.0F;
  const Rectangle badge{rightEdge - badgeWidth, badgeY, badgeWidth,
                        cfg::badgeHeight};
  DrawRectangleRounded(badge, cfg::badgeRoundness, cfg::badgeSegments,
                       background);
  DrawTextEx(font, label,
             Vector2{badge.x + cfg::badgePaddingX,
                     badgeY + (cfg::badgeHeight - textSize.y) / 2.0F},
             fsize, cfg::textSpacing, WHITE);
}

void drawTeamRow(const Rectangle& panel, float rowY, const std::string& team,
                 bool isWinner, const Font& font) {
  DrawRectangleRec(
      Rectangle{panel.x + cfg::panelPadding,
                rowY + cfg::panelRowHeight - cfg::rowSeparatorHeight,
                panel.width - cfg::panelPadding * 2.0F,
                cfg::rowSeparatorHeight},
      cfg::panelSeparator);
  const float rowCenterY = rowY + cfg::panelRowHeight / 2.0F;
  const float fsize = static_cast<float>(cfg::teamFontSize);
  const Vector2 textSize =
      MeasureTextEx(font, team.c_str(), fsize, cfg::textSpacing);
  DrawTextEx(font, team.c_str(),
             Vector2{panel.x + cfg::panelPadding + cfg::rowTextIndent,
                     rowCenterY - textSize.y / 2.0F},
             fsize, cfg::textSpacing,
             isWinner ? cfg::winColor : cfg::teamLoseColor);
  const Color badgeBg = isWinner ? cfg::badgeWinBg : cfg::badgeLoseBg;
  drawBadge(panel.x + panel.width - cfg::panelPadding, rowCenterY,
            isWinner ? "WINNER" : "DEFEATED", badgeBg, font);
}

}  // namespace

void TeamResultPanel::draw(const WorldState& world, const Font& font) {
  if (world.teams.empty()) {
    return;
  }
  const Rectangle panel = panelRect(world.teams.size());
  drawFrame(panel);
  drawHeader(panel, font);
  float rowY = panel.y + cfg::panelHeaderHeight;
  for (std::size_t i = 0; i < world.teams.size(); ++i) {
    drawTeamRow(panel, rowY, world.teams[i], world.teams[i] == world.winnerTeam,
                font);
    rowY += cfg::panelRowHeight;
  }
}

}  // namespace zappy::gui
