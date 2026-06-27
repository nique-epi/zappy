/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TeamProgressBar — level-distribution histogram for one team
*/

#include "Render/Stats/Chart/TeamProgressBar.hpp"
#include <raylib.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include "Render/Stats/StatsConfig.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

namespace {

std::array<int, cfg::STATS_MAX_LEVEL> collectLevelCounts(
    const WorldState& world, const std::string& teamName) {
  std::array<int, cfg::STATS_MAX_LEVEL> counts{};
  for (const auto& player : world.players) {
    if (player.alive && player.teamName == teamName) {
      const int index =
          std::clamp(player.level - 1, 0, cfg::STATS_MAX_LEVEL - 1);
      ++counts[static_cast<std::size_t>(index)];
    }
  }
  return counts;
}

}  // namespace

void drawTeamProgressBar(Vector2 origin, float width, const WorldState& world,
                         const std::string& teamName, Color barColor) {
  const auto counts = collectLevelCounts(world, teamName);
  const int maxCount = *std::ranges::max_element(counts);

  const float totalGaps =
      cfg::STATS_BAR_GAP * static_cast<float>(cfg::STATS_MAX_LEVEL - 1);
  const float barWidth =
      (width - totalGaps) / static_cast<float>(cfg::STATS_MAX_LEVEL);
  const float innerHeight = cfg::STATS_CHART_H - 2.0F;

  for (int i = 0; i < cfg::STATS_MAX_LEVEL; ++i) {
    const float barX =
        origin.x + (static_cast<float>(i) * (barWidth + cfg::STATS_BAR_GAP));

    DrawRectangleLinesEx({barX, origin.y + 1.0F, barWidth, innerHeight}, 1.0F,
                         cfg::STATS_CHART_EMPTY);

    const auto index = static_cast<std::size_t>(i);
    if (maxCount > 0 && counts[index] > 0) {
      const float fillRatio =
          static_cast<float>(counts[index]) / static_cast<float>(maxCount);
      const float fillHeight = fillRatio * innerHeight;
      DrawRectangleRounded({barX, origin.y + 1.0F + (innerHeight - fillHeight),
                            barWidth, fillHeight},
                           0.15F, 4,
                           Color{barColor.r, barColor.g, barColor.b, 220});
    }

    const std::array<char, 2> levelLabel = {static_cast<char>('1' + i), '\0'};
    const int labelWidth =
        MeasureText(levelLabel.data(), cfg::STATS_CHART_LABEL_FONT);
    DrawText(levelLabel.data(),
             static_cast<int>(
                 barX + ((barWidth - static_cast<float>(labelWidth)) / 2.0F)),
             static_cast<int>(origin.y + cfg::STATS_CHART_H +
                              cfg::STATS_CHART_LABEL_GAP),
             cfg::STATS_CHART_LABEL_FONT, cfg::STATS_CHART_LABEL_CLR);
  }
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

}  // namespace zappy::gui
