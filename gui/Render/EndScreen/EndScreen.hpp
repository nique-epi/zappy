/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EndScreen — game-over overlay orchestrating overlay, title and team panel
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <string>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Full-screen overlay rendered when the server signals game over.
 *
 * Draws on top of the running 3D view: dark vignette, "GAME OVER" title
 * in the menu KiwiSoda font, a team results panel (green winner / red
 * defeated), and a back-to-menu hint.  Stateless: redrawn every frame.
 * The font must be loaded once before the game session starts and
 * unloaded when it ends, mirroring IncantationRenderer::loadTextures().
 */
class EndScreen {
 public:
  static void loadFont();
  static void unloadFont();

  /**
   * @brief Draw the end-of-game overlay.
   *
   * @param[in] world    Live world state (winnerTeam, teams list).
   * @param[in] keyName  Human-readable name of the back-to-menu key.
   */
  static void draw(const WorldState& world, const std::string& keyName);
};

}  // namespace zappy::gui
