/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerSelection — tracks the spectator-selected player across frames
*/

#pragma once

#include <optional>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Holds which player the spectator has selected, if any.
 *
 * Pure selection state: it reads no input and draws nothing. The caller feeds
 * it resolved per-frame facts and queries the result.
 */
class PlayerSelection {
 public:
  /**
   * @brief Apply a left click.
   *
   * @param[in] hoveredId        Player under the cursor, or none.
   * @param[in] clickInsidePanel Whether the click landed on the detail panel.
   * @returns The newly selected player id when the selection changed to a
   *          player (so the caller can refresh it), otherwise none.
   */
  std::optional<int> click(std::optional<int> hoveredId, bool clickInsidePanel);

  /**
   * @brief Drop the selection if the selected player vanished or died.
   */
  void syncWithWorld(const WorldState& world);

  /**
   * @brief Clear the selection (e.g. the close button was pressed).
   */
  void close();

  /**
   * @brief Currently selected player id, or none.
   */
  [[nodiscard]] std::optional<int> selectedId() const;

 private:
  std::optional<int> selectedId_;
};

}  // namespace zappy::gui
