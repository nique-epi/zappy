/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** StatsPanel — live statistics panel with per-team progression charts
*/

#pragma once

#include <raylib.h>
#include <string>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Toggle-able live statistics panel with per-team progression charts.
 *
 * Renders a small icon button in the bottom-right corner styled like the
 * main-menu buttons. The icon scales up on hover and shows a "Statistics"
 * tooltip. When toggled open, a raygui-style panel displays per-team level
 * histograms and global resource totals, refreshed every frame.
 */
class StatsPanel {
 public:
  /**
   * @brief Draw the icon toggle button and, when open, the full panel.
   */
  void draw(const WorldState& world);

  /**
   * @brief Whether @p point falls inside the open panel or the icon button.
   */
  [[nodiscard]] bool contains(Vector2 point) const;

 private:
  bool isOpen_{false};
  Rectangle lastPanelRect_{};

  [[nodiscard]] static Rectangle buttonRect();
  [[nodiscard]] static float computePanelHeight(const WorldState& world);
  [[nodiscard]] static Rectangle panelRect(const WorldState& world);

  void drawIconButton();
  void renderIconButton(const Rectangle& drawRect, float centerX,
                        float logicalY, bool held, bool hovered) const;
  static void drawChartIcon(const Rectangle& rect);
  void drawPanel(const WorldState& world);
  static void drawSectionHeader(float contentX, float& cursor,
                                const char* title);
  static void drawTeamBlock(const Rectangle& panel, float& cursor,
                            const WorldState& world,
                            const std::string& teamName);
  static void drawGlobalBlock(const Rectangle& panel, float& cursor,
                              const WorldState& world);
};

}  // namespace zappy::gui
