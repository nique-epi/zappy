/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PortInputDialog
*/

#include "Menu/Dialogs/PortInputDialog.hpp"
#include <raylib.h>
#include <charconv>
#include <cstring>
#include <system_error>
#include "GuiConfig.hpp"
#include "Menu/MenuConfig.hpp"
#include "Render/raygui.h"

namespace zappy::gui {

namespace cfg = menu::config;

PortInputDialog::
    PortInputDialog() {  // NOLINT(hicpp-use-equals-default,modernize-use-equals-default,cppcoreguidelines-pro-type-member-init,hicpp-member-init)
  std::strncpy(hostBuffer_.data(), "localhost", cfg::textBufferSize - 1);
  hostBuffer_[cfg::textBufferSize - 1] = '\0';
  std::strncpy(portBuffer_.data(), "4242", cfg::textBufferSize - 1);
  portBuffer_[cfg::textBufferSize - 1] = '\0';
}

void PortInputDialog::open() {
  justOpened_ = true;
  portInvalid_ = false;
  connectionError_.clear();
}

void PortInputDialog::setConnectionError(const std::string& message) {
  connectionError_ = message;
  justOpened_ = true;
}

void PortInputDialog::drawHostField(float contentX, float contentWidth,
                                    float& cursorY) {
  GuiLabel({contentX, cursorY, contentWidth, cfg::dialogLabelHeight},
           "Server hostname:");
  cursorY += cfg::dialogLabelHeight + cfg::dialogSmallGap;

  if (GuiTextBox({contentX, cursorY, contentWidth, cfg::dialogInputHeight},
                 hostBuffer_.data(), cfg::textBufferSize,
                 hostEditActive_) != 0) {
    hostEditActive_ = !hostEditActive_;
    portEditActive_ = false;
    connectionError_.clear();
  }
  cursorY += cfg::dialogInputHeight + cfg::dialogGap;
}

void PortInputDialog::drawPortField(float contentX, float contentWidth,
                                    float& cursorY) {
  GuiLabel({contentX, cursorY, contentWidth, cfg::dialogLabelHeight}, "Port:");
  if (portInvalid_) {
    DrawText(
        "Invalid port (1-65535)",
        static_cast<int>(contentX + contentWidth - cfg::portInvalidTextOffset),
        static_cast<int>(cursorY), cfg::dialogErrorFontSize, RED);
  }
  cursorY += cfg::dialogLabelHeight + cfg::dialogSmallGap;

  if (GuiTextBox({contentX, cursorY, contentWidth, cfg::dialogInputHeight},
                 portBuffer_.data(), cfg::textBufferSize,
                 portEditActive_) != 0) {
    portEditActive_ = !portEditActive_;
    hostEditActive_ = false;
    connectionError_.clear();
  }
  cursorY += cfg::dialogInputHeight + cfg::dialogGap;
}

void PortInputDialog::drawConnectionError(float contentX, float cursorY) const {
  if (!connectionError_.empty()) {
    DrawText(connectionError_.c_str(), static_cast<int>(contentX),
             static_cast<int>(cursorY), cfg::dialogHintFontSize, RED);
  }
}

DialogResult PortInputDialog::drawActionButtons(float contentX,
                                                float contentWidth,
                                                float cursorY) {
  const float halfWidth = (contentWidth - cfg::dialogGap) / 2.0F;
  const bool canInteract = !justOpened_;
  justOpened_ = false;

  const bool connectPressed =
      ((GuiButton({contentX, cursorY, halfWidth, cfg::dialogButtonHeight},
                  "Connect") != 0) &&
       canInteract) ||
      (IsKeyPressed(KEY_ENTER) && (portEditActive_ || hostEditActive_));
  const bool cancelPressed =
      (GuiButton({contentX + halfWidth + cfg::dialogGap, cursorY, halfWidth,
                  cfg::dialogButtonHeight},
                 "Cancel") != 0) &&
      canInteract;

  if (cancelPressed) {
    portInvalid_ = false;
    connectionError_.clear();
    return DialogResult::Cancelled;
  }
  if (connectPressed) {
    return tryConnect();
  }
  return DialogResult::Open;
}

DialogResult PortInputDialog::tryConnect() {
  connectionError_.clear();
  int port = 0;            // NOLINT(misc-const-correctness)
  const char* const end =  // NOLINT(cppcoreguidelines-init-variables)
      portBuffer_.data() + std::strlen(portBuffer_.data());
  const auto [ptr, ec] = std::from_chars(portBuffer_.data(), end, port);
  if (ec != std::errc{} || port < portMin || port > portMax) {
    portInvalid_ = true;
    return DialogResult::Open;
  }
  portInvalid_ = false;
  result_ =
      GuiConfig{.port = port, .hostname = std::string(hostBuffer_.data())};
  return DialogResult::Connected;
}

DialogResult PortInputDialog::draw() {
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                Color{0, 0, 0, static_cast<unsigned char>(cfg::overlayAlpha)});

  const Rectangle panel{cfg::dialogX, cfg::dialogY, cfg::dialogWidth,
                        cfg::dialogHeight};
  GuiPanel(panel, "Connect to server");

  const float contentX = panel.x + cfg::dialogPadding;
  const float contentWidth = panel.width - (2.0F * cfg::dialogPadding);
  float cursorY = panel.y + cfg::dialogPanelHeader + cfg::dialogGap;

  drawHostField(contentX, contentWidth, cursorY);
  drawPortField(contentX, contentWidth, cursorY);
  drawConnectionError(contentX, cursorY);
  cursorY += cfg::dialogLabelHeight + cfg::dialogGap;

  return drawActionButtons(contentX, contentWidth, cursorY);
}

const GuiConfig& PortInputDialog::result() const { return result_; }

}  // namespace zappy::gui
