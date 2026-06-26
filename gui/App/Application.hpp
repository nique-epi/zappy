/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Application — owns the window and the menu/session orchestration loop
*/

#pragma once

namespace zappy::gui {

/**
 * @brief Top-level GUI application.
 *
 * Owns the raylib window lifetime and the outer loop that alternates between
 * the menu and a connected @ref GameSession, translating any escaping
 * exception into a process exit code.
 */
class Application {
 public:
  Application(int argumentCount, char** arguments);

  /**
   * @brief Run the application to completion.
   *
   * @returns 0 on a clean exit, 84 on a fatal error.
   */
  int run();

 private:
  int argumentCount_;
  char** arguments_;
};

}  // namespace zappy::gui
