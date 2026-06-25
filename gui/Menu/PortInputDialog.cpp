/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PortInputDialog
*/

#include "Menu/PortInputDialog.hpp"
#include <raylib.h>
#include <charconv>
#include <cstring>
#include "Menu/MenuConfig.hpp"
#include "Render/raygui.h"

namespace zappy::gui {

namespace cfg = menu::config;

PortInputDialog::PortInputDialog() {
  std::strncpy(hostBuffer_, "localhost", cfg::textBufferSize - 1);
  hostBuffer_[cfg::textBufferSize - 1] = '\0';
  std::strncpy(portBuffer_, "4242", cfg::textBufferSize - 1);
  portBuffer_[cfg::textBufferSize - 1] = '\0';
}

void PortInputDialog::setConnectionError(const std::string& message) {
  connectionError_ = message;
}

DialogResult PortInputDialog::draw() {
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                Color{0, 0, 0, static_cast<unsigned char>(cfg::overlayAlpha)});

  const Rectangle panel{cfg::dialogX, cfg::dialogY, cfg::dialogWidth,
                        cfg::dialogHeight};
  GuiPanel(panel, "Connect to server");

  const float contentX = panel.x + cfg::dialogPadding;
  const float contentWidth = panel.width - 2.0F * cfg::dialogPadding;
  float cursorY = panel.y + 28.0F + cfg::dialogGap;

  GuiLabel(Rectangle{contentX, cursorY, contentWidth, cfg::dialogLabelHeight},
           "Server hostname:");
  cursorY += cfg::dialogLabelHeight + 4.0F;

  const Rectangle hostRect{contentX, cursorY, contentWidth,
                           cfg::dialogInputHeight};
  if (GuiTextBox(hostRect, hostBuffer_, cfg::textBufferSize, hostEditActive_)) {
    hostEditActive_ = !hostEditActive_;
    portEditActive_ = false;
    connectionError_.clear();
  }
  cursorY += cfg::dialogInputHeight + cfg::dialogGap;

  GuiLabel(Rectangle{contentX, cursorY, contentWidth, cfg::dialogLabelHeight},
           "Port:");
  if (portInvalid_) {
    DrawText("Invalid port (1-65535)",
             static_cast<int>(contentX + contentWidth - 195),
             static_cast<int>(cursorY), 14, RED);
  }
  cursorY += cfg::dialogLabelHeight + 4.0F;

  const Rectangle portRect{contentX, cursorY, contentWidth,
                           cfg::dialogInputHeight};
  if (GuiTextBox(portRect, portBuffer_, cfg::textBufferSize, portEditActive_)) {
    portEditActive_ = !portEditActive_;
    hostEditActive_ = false;
    connectionError_.clear();
  }
  cursorY += cfg::dialogInputHeight + cfg::dialogGap;

  if (!connectionError_.empty()) {
    DrawText(connectionError_.c_str(), static_cast<int>(contentX),
             static_cast<int>(cursorY), 13, RED);
  }
  cursorY += cfg::dialogLabelHeight + cfg::dialogGap;

  const float halfWidth = (contentWidth - cfg::dialogGap) / 2.0F;
  const bool connectPressed =
      GuiButton(
          Rectangle{contentX, cursorY, halfWidth, cfg::dialogButtonHeight},
          "Connect") ||
      (IsKeyPressed(KEY_ENTER) && (portEditActive_ || hostEditActive_));
  const bool cancelPressed =
      GuiButton(Rectangle{contentX + halfWidth + cfg::dialogGap, cursorY,
                          halfWidth, cfg::dialogButtonHeight},
                "Cancel");

  if (cancelPressed) {
    portInvalid_ = false;
    connectionError_.clear();
    return DialogResult::Cancelled;
  }

  if (connectPressed) {
    connectionError_.clear();
    int port = 0;
    const char* end = portBuffer_ + std::strlen(portBuffer_);
    const auto [ptr, ec] = std::from_chars(portBuffer_, end, port);
    if (ec != std::errc{} || port < portMin || port > portMax) {
      portInvalid_ = true;
    } else {
      portInvalid_ = false;
      result_ = GuiConfig{.port = port, .hostname = std::string(hostBuffer_)};
      return DialogResult::Connected;
    }
  }

  return DialogResult::Open;
}

const GuiConfig& PortInputDialog::result() const { return result_; }

}  // namespace zappy::gui
