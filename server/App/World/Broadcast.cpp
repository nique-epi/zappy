/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Broadcast — tile a drone hears an anonymous sound coming from
*/

#include "App/World/Broadcast.hpp"
#include <cmath>
#include <numbers>
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Direction.hpp"

namespace zappy::world {

namespace {

constexpr int sameTileCode = 0;
constexpr int soundTileCount = 8;

int shortestSignedOffset(int rawDelta, int modulus) {
  const int wrapped = Map::wrap(rawDelta, modulus);
  if (wrapped * 2 > modulus) {
    return wrapped - modulus;
  }
  return wrapped;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
int broadcastSoundDirection(int emitterX, int emitterY, int receiverX,
                            int receiverY, Direction receiverFacing, int width,
                            int height) {
  const int offsetX = shortestSignedOffset(emitterX - receiverX, width);
  const int offsetY = shortestSignedOffset(emitterY - receiverY, height);
  if (offsetX == 0 && offsetY == 0) {
    return sameTileCode;
  }
  const DirectionOffset forward = directionOffset(receiverFacing);
  const int forwardComponent = (offsetX * forward.dx) + (offsetY * forward.dy);
  const int leftComponent = (offsetX * forward.dy) - (offsetY * forward.dx);
  const double angle = std::atan2(static_cast<double>(leftComponent),
                                  static_cast<double>(forwardComponent));
  const int sector =
      static_cast<int>(std::lround(angle / (std::numbers::pi / 4.0)));
  return Map::wrap(sector, soundTileCount) + 1;
}

}  // namespace zappy::world
