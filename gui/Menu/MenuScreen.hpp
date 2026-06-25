/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuScreen — main menu with 3D simulation background
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <raylib.h>
#include <cstdint>
#include <optional>
#include <vector>
#include "Bindings/KeyBindings.hpp"
#include "GuiConfig.hpp"  // NOLINT(misc-include-cleaner)
#include "Menu/KeyBindingsDialog.hpp"
#include "Menu/MenuButton.hpp"
#include "Menu/MenuCamera.hpp"
#include "Menu/PortInputDialog.hpp"
#include "Render/SkyRenderer.hpp"  // NOLINT(misc-include-cleaner)
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
  ~MenuScreen();

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
   */
  void showConnectionError(const std::string& message);

  /**
   * @brief Resets the menu to its initial state (buttons visible, no dialog
   * open).
   */
  void reset();

  /**
   * @brief Returns the currently committed key bindings.
   */
  [[nodiscard]] const KeyBindings& keyBindings() const;

 private:
  enum class State : std::uint8_t { Menu, ConnectDialog, EditBindings };

  void drawSimulation();
  void drawTitle() const;

  WorldState world_;
  MenuCamera camera_;

  Font titleFont_;

  MenuButton playButton_;
  MenuButton controlsButton_;
  MenuButton exitButton_;

  PortInputDialog portDialog_;
  KeyBindingsDialog kbDialog_;
  State state_{State::Menu};
};

}  // namespace zappy::gui
