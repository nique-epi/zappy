/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EndBackground — dark overlay and title for the end-of-game screen
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <raylib.h>
#include <string>

namespace zappy::gui {

/**
 * @brief Draws the fullscreen semi-transparent dark overlay.
 */
void drawEndOverlay();

/**
 * @brief Draws "GAME OVER" and the winner's team name using the menu font.
 *
 * @param[in] font       KiwiSoda font pre-loaded by EndScreen.
 * @param[in] winnerTeam Name of the winning team.
 */
void drawEndTitle(const Font& font, const std::string& winnerTeam);

/**
 * @brief Draws the "press <key> to return" hint at the bottom of the screen.
 *
 * @param[in] font   KiwiSoda font pre-loaded by EndScreen.
 * @param[in] keyName Human-readable name of the back-to-menu key.
 */
void drawEndHint(const Font& font, const std::string& keyName);

}  // namespace zappy::gui
