/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PortInputDialog — raygui modal for server host + port entry
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include "GuiConfig.hpp"
#include "Menu/MenuConfig.hpp"  // NOLINT(misc-include-cleaner)

namespace zappy::gui {

enum class DialogResult : std::uint8_t { Open, Cancelled, Connected };

/**
 * @brief Immediate-mode overlay dialog that collects hostname and port.
 *
 * Call draw() every frame while the dialog is visible. Inspect the return
 * value to know whether to keep the dialog open, close it, or start the game.
 * When draw() returns Connected, call result() to retrieve the validated
 * config.
 */
class PortInputDialog {
 public:
  PortInputDialog();

  /**
   * @brief Draws the dialog and processes input.
   *
   * @returns Open while waiting, Cancelled if the user dismissed, Connected on
   *          successful validation.
   */
  void open();
  DialogResult draw();

  [[nodiscard]] const GuiConfig& result() const;

  /**
   * @brief Displays a connection-level error (e.g. "Connection refused") on the
   * next draw.
   */
  void setConnectionError(const std::string& message);

 private:
  static constexpr int portMin = 1;
  static constexpr int portMax = 65535;

  void drawHostField(float contentX, float contentWidth, float& cursorY);
  void drawPortField(float contentX, float contentWidth, float& cursorY);
  void drawConnectionError(float contentX, float cursorY) const;
  DialogResult drawActionButtons(float contentX, float contentWidth,
                                 float cursorY);
  DialogResult tryConnect();

  std::array<char, menu::config::textBufferSize> hostBuffer_{};
  std::array<char, menu::config::textBufferSize> portBuffer_{};
  bool hostEditActive_{false};
  bool portEditActive_{false};
  bool portInvalid_{false};
  bool justOpened_{true};
  std::string connectionError_;
  GuiConfig result_{};
};

}  // namespace zappy::gui
