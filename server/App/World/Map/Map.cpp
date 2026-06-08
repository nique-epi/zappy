/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Map — toroidal grid of tiles forming the world surface
*/

#include "App/World/Map/Map.hpp"
#include "App/World/Exceptions/WorldException.hpp"

namespace zappy::world {

Map::Map(int width, int height) : width_(width), height_(height) {
  if (width <= 0 || height <= 0) {
    throw InvalidMapDimensionsException(width, height);
  }
  tiles_.resize(static_cast<std::size_t>(width) *
                static_cast<std::size_t>(height));
}

int Map::width() const { return width_; }

int Map::height() const { return height_; }

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::size_t Map::indexOf(int column, int row) const {
  const int wrappedColumn = ((column % width_) + width_) % width_;
  const int wrappedRow = ((row % height_) + height_) % height_;

  return (static_cast<std::size_t>(wrappedRow) *
          static_cast<std::size_t>(width_)) +
         static_cast<std::size_t>(wrappedColumn);
}

Tile &Map::tileAt(int column, int row) { return tiles_[indexOf(column, row)]; }

const Tile &Map::tileAt(int column, int row) const {
  return tiles_[indexOf(column, row)];
}

}  // namespace zappy::world
