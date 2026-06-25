/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuConfig — layout and rendering constants for the main menu
*/

#pragma once  // NOLINT(llvm-header-guard)

namespace zappy::gui::menu::config {

constexpr int simulationMapSize = 14;

constexpr float cameraDistance = 28.0F;
constexpr float cameraPitchDegrees = 52.0F;
constexpr float cameraYawSpeedDegrees = 6.0F;
constexpr float cameraFieldOfViewDegrees = 45.0F;

constexpr float buttonWidth = 380.0F;
constexpr float buttonHeight = 126.7F;
constexpr float buttonX = (1280.0F - buttonWidth) / 2.0F;
constexpr float playButtonY = 285.0F;
constexpr float controlsButtonY = 420.0F;
constexpr float exitButtonY = 555.0F;

constexpr int titleFontSize = 120;
constexpr float titleY = 60.0F;

constexpr unsigned char menuBackgroundOverlay = 110U;
constexpr float overlayAlpha = 150.0F;

constexpr float dialogWidth = 500.0F;
constexpr float dialogHeight = 300.0F;
constexpr float dialogX = (1280.0F - dialogWidth) / 2.0F;
constexpr float dialogY = (720.0F - dialogHeight) / 2.0F;
constexpr float dialogPadding = 18.0F;
constexpr float dialogLabelHeight = 20.0F;
constexpr float dialogInputHeight = 36.0F;
constexpr float dialogButtonHeight = 38.0F;
constexpr float dialogGap = 10.0F;
constexpr float dialogPanelHeader = 28.0F;
constexpr float dialogSmallGap = 4.0F;

constexpr int portInvalidTextOffset = 195;
constexpr int dialogErrorFontSize = 14;
constexpr int dialogHintFontSize = 13;

constexpr int textBufferSize = 64;

constexpr int controlsDialogWidth = 440;
constexpr int controlsDialogHeight = 240;

}  // namespace zappy::gui::menu::config
