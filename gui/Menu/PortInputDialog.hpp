/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PortInputDialog — raygui modal for server host + port entry
*/

#pragma once

#include <optional>
#include "GuiConfig.hpp"
#include "Menu/MenuConfig.hpp"

namespace zappy::gui {

enum class DialogResult { Open, Cancelled, Connected };

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

  char hostBuffer_[menu::config::textBufferSize];
  char portBuffer_[menu::config::textBufferSize];
  bool hostEditActive_{false};
  bool portEditActive_{false};
  bool portInvalid_{false};
  std::string connectionError_;
  GuiConfig result_{};
};

}  // namespace zappy::gui
