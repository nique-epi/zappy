/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EndScreenConfig — layout and rendering constants for the end-of-game screen
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <raylib.h>

namespace zappy::gui::endscreen::config {

constexpr int titleFontSize = 90;
constexpr float titleY = 80.0F;

constexpr float panelWidth = 520.0F;
constexpr float panelYRatio = 0.30F;
constexpr float panelPadding = 20.0F;
constexpr float panelHeaderHeight = 50.0F;
constexpr float panelRowHeight = 52.0F;
constexpr float panelBottomPad = 20.0F;
constexpr float panelRoundness = 0.4F;
constexpr int panelSegments = 10;
constexpr float panelBorderThickness = 3.0F;
constexpr float panelShadowOffset = 6.0F;

constexpr float rowSeparatorHeight = 1.0F;
constexpr float badgeHeight = 24.0F;
constexpr float badgePaddingX = 12.0F;
constexpr float badgeRoundness = 0.4F;
constexpr int badgeSegments = 10;
constexpr int badgeFontSize = 15;

constexpr float textSpacing = 1.5F;
constexpr float rowTextIndent = 16.0F;
constexpr int teamFontSize = 22;
constexpr int headerFontSize = 18;
constexpr int hintFontSize = 15;
constexpr float hintBottomMargin = 32.0F;

constexpr unsigned char overlayAlpha = 180U;

constexpr Color overlayColor = {8, 8, 18, overlayAlpha};
constexpr Color panelBg = {245, 232, 210, 255};
constexpr Color panelBorder = {101, 67, 33, 255};
constexpr Color panelShadow = {0, 0, 0, 80};
constexpr Color panelSeparator = {180, 155, 120, 255};
constexpr Color titleColor = {236, 239, 244, 255};
constexpr Color headerColor = {101, 67, 33, 255};
constexpr Color teamTextColor = {80, 50, 25, 255};
constexpr Color teamLoseColor = {140, 110, 80, 255};
constexpr Color winColor = {40, 140, 60, 255};
constexpr Color hintColor = {200, 190, 170, 200};
constexpr Color badgeWinBg = {40, 130, 60, 255};
constexpr Color badgeLoseBg = {101, 67, 33, 255};

}  // namespace zappy::gui::endscreen::config
