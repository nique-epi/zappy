/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** KeyBindingsDialog
*/

#include "Menu/KeyBindingsDialog.hpp"
#include <raylib.h>
#include <array>
#include <cstddef>
#include <string>
#include <utility>
#include "Bindings/KeyBindings.hpp"
#include "Render/WindowConfig.hpp"
#include "Render/raygui.h"

namespace zappy::gui {

namespace cfg = config;

namespace {

struct RowDef {
  const char* label;
  int KeyBindings::* field;
};

constexpr std::array<RowDef, 5> kRows{{
    {.label = "Camera Forward  (up arrow always works)",
     .field = &KeyBindings::cameraForward},
    {.label = "Camera Backward (down arrow always works)",
     .field = &KeyBindings::cameraBackward},
    {.label = "Camera Left     (left arrow always works)",
     .field = &KeyBindings::cameraLeft},
    {.label = "Camera Right    (right arrow always works)",
     .field = &KeyBindings::cameraRight},
    {.label = "Back to Menu", .field = &KeyBindings::backToMenu},
}};

constexpr float dialogWidth = 740.0F;
constexpr float dialogHeight = 440.0F;
constexpr float dialogX = (cfg::WINDOW_WIDTH - dialogWidth) / 2.0F;
constexpr float dialogY = (cfg::WINDOW_HEIGHT - dialogHeight) / 2.0F;
constexpr float padding = 18.0F;
constexpr float panelHeader = 28.0F;
constexpr float rowHeight = 34.0F;
constexpr float rowGap = 10.0F;
constexpr float labelWidth = 430.0F;
constexpr float labelKeyGap = 14.0F;
constexpr float btnHeight = 38.0F;
constexpr unsigned char overlayOpacity = 150U;
constexpr int errorFontSize = 14;
constexpr float errorLineHeight = 22.0F;
constexpr int hintFontSize = 12;
constexpr float hintLineHeight = 28.0F;
constexpr float popupWidth = 400.0F;
constexpr float popupHeight = 160.0F;

}  // namespace

void KeyBindingsDialog::open() {
  draft_ = committed_;
  state_ = InternalState::Idle;
  listeningRow_ = -1;
  justOpened_ = true;
  conflictError_.clear();
}

const KeyBindings& KeyBindingsDialog::result() const { return committed_; }

bool KeyBindingsDialog::hasConflict() const {
  for (std::size_t i = 0; i < kRows.size();
       ++i) {  // NOLINT(readability-use-anyofallof)
    for (std::size_t j = i + 1; j < kRows.size(); ++j) {
      if (draft_.*kRows[i].field == draft_.*kRows[j].field) {
        return true;
      }
    }
  }
  return false;
}

bool KeyBindingsDialog::hasUnsavedChanges()
    const {  // NOLINT(readability-convert-member-functions-to-static)
  for (const auto& row : kRows) {  // NOLINT(readability-use-anyofallof)
    if (draft_.*row.field != committed_.*row.field) {
      return true;
    }
  }
  return false;
}

void KeyBindingsDialog::pollKeyCapture() {
  if (listeningRow_ == -1) {
    return;
  }
  const int key = GetKeyPressed();
  if (key != KEY_NULL) {
    draft_.*kRows[static_cast<std::size_t>(listeningRow_)].field = key;
    listeningRow_ = -1;
    conflictError_.clear();
  }
}

void KeyBindingsDialog::drawBindingRows(float contentX, float keyBtnWidth,
                                        float& cursorY) {
  for (int i = 0; std::cmp_less(i, kRows.size()); ++i) {
    const auto idx = static_cast<std::size_t>(i);
    const bool isListen = (listeningRow_ == i);

    GuiLabel({contentX, cursorY, labelWidth, rowHeight}, kRows[idx].label);

    const std::string keyText =
        isListen ? "[Press any key]" : keyName(draft_.*kRows[idx].field);

    if (GuiButton({contentX + labelWidth + labelKeyGap, cursorY, keyBtnWidth,
                   rowHeight},
                  keyText.c_str()) != 0) {
      listeningRow_ = isListen ? -1 : i;
    }

    cursorY += rowHeight + rowGap;
  }
}

void KeyBindingsDialog::drawFooterText(float contentX, float& cursorY) const {
  if (!conflictError_.empty()) {
    DrawText(conflictError_.c_str(), static_cast<int>(contentX),
             static_cast<int>(cursorY), errorFontSize, RED);
  }
  cursorY += errorLineHeight;

  DrawText(
      "Arrow keys always work as camera alternates  |  "
      "Zoom: mouse wheel  |  Rotate: right-click drag",
      static_cast<int>(contentX), static_cast<int>(cursorY), hintFontSize,
      GRAY);
  cursorY += hintLineHeight;
}

BindingsResult KeyBindingsDialog::drawActionButtons(float contentX, float halfW,
                                                    float cursorY,
                                                    bool canInteract) {
  BindingsResult out = BindingsResult::Open;

  if ((GuiButton({contentX, cursorY, halfW, btnHeight}, "Apply") != 0) &&
      canInteract) {
    if (hasConflict()) {
      conflictError_ = "Conflict: two actions use the same key!";
    } else {
      committed_ = draft_;
      conflictError_.clear();
      out = BindingsResult::Closed;
    }
  }

  if ((GuiButton({contentX + halfW + rowGap, cursorY, halfW, btnHeight},
                 "Back") != 0) &&
      canInteract) {
    if (hasUnsavedChanges()) {
      state_ = InternalState::ConfirmDiscard;
    } else {
      out = BindingsResult::Closed;
    }
  }

  return out;
}

BindingsResult KeyBindingsDialog::drawDiscardPopup() {
  const float popX = (cfg::WINDOW_WIDTH - popupWidth) / 2.0F;
  const float popY = (cfg::WINDOW_HEIGHT - popupHeight) / 2.0F;

  const int choice = GuiMessageBox(
      {popX, popY, popupWidth, popupHeight}, "Unsaved Changes",
      "Some changes haven't been applied.\nGo back anyway?", "Yes;No");

  if (choice == 0 || choice == 1) {
    draft_ = committed_;
    state_ = InternalState::Idle;
    return BindingsResult::Closed;
  }
  if (choice == 2) {
    state_ = InternalState::Idle;
  }
  return BindingsResult::Open;
}

BindingsResult KeyBindingsDialog::draw() {
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                Color{0, 0, 0, overlayOpacity});

  const Rectangle panel{dialogX, dialogY, dialogWidth, dialogHeight};
  GuiPanel(panel, "Key Bindings");

  const float contentX = panel.x + padding;
  const float contentW = panel.width - (2.0F * padding);
  const float keyBtnWidth = contentW - labelWidth - labelKeyGap;
  float cursorY = panel.y + panelHeader + padding;

  pollKeyCapture();
  drawBindingRows(contentX, keyBtnWidth, cursorY);
  cursorY += rowGap;
  drawFooterText(contentX, cursorY);

  const bool canInteract =
      !justOpened_ && (listeningRow_ == -1) && (state_ == InternalState::Idle);
  justOpened_ = false;
  const float halfW = (contentW - rowGap) / 2.0F;

  BindingsResult out = drawActionButtons(contentX, halfW, cursorY, canInteract);
  if (state_ == InternalState::ConfirmDiscard) {
    out = drawDiscardPopup();
  }
  return out;
}

}  // namespace zappy::gui
