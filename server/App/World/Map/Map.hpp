/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Map — toroidal grid of tiles forming the world surface
*/

#pragma once

#include <cstddef>
#include <vector>
#include "App/World/Tile/Tile.hpp"

namespace zappy::world {

/**
 * @brief The world surface: a `width × height` grid of tiles with toroidal
 *        topology.
 *
 * The surface has no relief; every cell is a Tile. Coordinates wrap on both
 * axes — and therefore at the four corners: leaving the grid through any edge
 * re-enters it on the opposite edge. Any integer coordinate, negative or past
 * the bounds, maps to a valid tile, so movement and vision never have to
 * bounds-check before a lookup.
 */
class Map {
 public:
  /**
   * @brief Build a @p width × @p height world of empty tiles.
   * @param[in] width  Number of columns, the X axis (`-x`); must be > 0.
   * @param[in] height Number of rows, the Y axis (`-y`); must be > 0.
   * @throws InvalidMapDimensionsException if either dimension is not strictly
   *         positive.
   */
  Map(int width, int height);

  /// Number of columns along the X axis.
  [[nodiscard]] int width() const;

  /// Number of rows along the Y axis.
  [[nodiscard]] int height() const;

  /**
   * @brief Normalise @p value into `[0, modulus)` with toroidal wrapping.
   * @param[in] value   Any integer, negative or past the bounds.
   * @param[in] modulus Strictly positive extent of the axis.
   * @returns @p value mapped into `[0, modulus)`.
   */
  [[nodiscard]] static int wrap(int value, int modulus);

  /**
   * @brief Access the tile at (@p column, @p row), wrapping toroidally.
   * @param[in] column Position on the X axis; any integer, normalised modulo
   *                   width.
   * @param[in] row    Position on the Y axis; any integer, normalised modulo
   *                   height.
   * @returns A reference to the tile at the wrapped coordinates.
   */
  [[nodiscard]] Tile& tileAt(int column, int row);

  /**
   * @brief Read-only access to the tile at (@p column, @p row), wrapping
   *        toroidally.
   * @param[in] column Position on the X axis; any integer, normalised modulo
   *                   width.
   * @param[in] row    Position on the Y axis; any integer, normalised modulo
   *                   height.
   * @returns A const reference to the tile at the wrapped coordinates.
   */
  [[nodiscard]] const Tile& tileAt(int column, int row) const;

 private:
  /**
   * @brief Flat index into @ref tiles_ for the toroidally wrapped
   *        (@p column, @p row).
   */
  [[nodiscard]] std::size_t indexOf(int column, int row) const;

  int width_;
  int height_;
  std::vector<Tile> tiles_;
};

}  // namespace zappy::world
