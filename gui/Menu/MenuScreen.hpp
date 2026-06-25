/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuScreen — main menu with 3D simulation background
*/

#pragma once

#include <optional>
#include "GuiConfig.hpp"
#include "Menu/MenuButton.hpp"
#include "Menu/MenuCamera.hpp"
#include "Menu/PortInputDialog.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Self-contained main menu that renders a 3D tile simulation as
 * background.
 *
 * Call run() after InitWindow(). It owns its own render loop and returns when
 * the user either starts the game or quits.
 */
class MenuScreen {
 public:
  MenuScreen();
  ~MenuScreen() = default;

  MenuScreen(const MenuScreen&) = delete;
  MenuScreen& operator=(const MenuScreen&) = delete;
  MenuScreen(MenuScreen&&) = delete;
  MenuScreen& operator=(MenuScreen&&) = delete;

  /**
   * @brief Blocks until the user selects Play (returns GuiConfig) or exits
   * (returns nullopt).
   */
  std::optional<GuiConfig> run();

  /**
   * @brief Re-opens the connect dialog with a connection-level error message.
   *
   * Call this after run() returns and the connection attempt fails, then call
   * run() again so the user can fix the host/port without leaving the menu.
   */
  void showConnectionError(const std::string& message);

  /**
   * @brief Resets the menu to its initial state (buttons visible, no dialog
   * open).
   *
   * Call this before re-entering run() after a game session ends.
   */
  void reset();

 private:
  enum class State { Menu, ConnectDialog, ShowControls };

  void drawSimulation();
  void drawTitle() const;
  void drawControlsDialog();

  WorldState world_;
  MenuCamera camera_;

  MenuButton playButton_;
  MenuButton controlsButton_;
  MenuButton exitButton_;

  PortInputDialog portDialog_;
  State state_{State::Menu};
};

}  // namespace zappy::gui
