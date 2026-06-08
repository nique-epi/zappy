/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the toroidal Map
*/

#include <gtest/gtest.h>
#include "App/World/Exceptions/WorldException.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Resources/ResourceType.hpp"

using zappy::world::InvalidMapDimensionsException;
using zappy::world::Map;
using zappy::world::MapAllocationException;
using zappy::world::ResourceType;
using zappy::world::WorldException;

TEST(Map, KeepsCommandLineDimensions) {
  const Map map(12, 7);

  EXPECT_EQ(map.width(), 12);
  EXPECT_EQ(map.height(), 7);
}

TEST(Map, RejectsNonPositiveDimensions) {
  EXPECT_THROW(Map(0, 10), InvalidMapDimensionsException);
  EXPECT_THROW(Map(10, 0), InvalidMapDimensionsException);
  EXPECT_THROW(Map(-1, 10), InvalidMapDimensionsException);
  EXPECT_THROW(Map(10, -3), InvalidMapDimensionsException);
}

TEST(Map, InvalidDimensionsAreCatchableAsWorldException) {
  EXPECT_THROW(Map(0, 0), WorldException);
}

TEST(Map, TranslatesAllocationFailureToWorldException) {
  EXPECT_THROW(Map(2000000000, 2000000000), MapAllocationException);
  EXPECT_THROW(Map(2000000000, 2000000000), WorldException);
}

TEST(Map, DistinctCoordinatesAreDistinctTiles) {
  Map map(10, 10);

  EXPECT_NE(&map.tileAt(0, 0), &map.tileAt(1, 0));
  EXPECT_NE(&map.tileAt(0, 0), &map.tileAt(0, 1));
}

TEST(Map, WrapsOnTheXAxis) {
  Map map(10, 6);

  EXPECT_EQ(&map.tileAt(10, 2), &map.tileAt(0, 2));
  EXPECT_EQ(&map.tileAt(-1, 2), &map.tileAt(9, 2));
  EXPECT_EQ(&map.tileAt(23, 2), &map.tileAt(3, 2));
}

TEST(Map, WrapsOnTheYAxis) {
  Map map(10, 6);

  EXPECT_EQ(&map.tileAt(4, 6), &map.tileAt(4, 0));
  EXPECT_EQ(&map.tileAt(4, -1), &map.tileAt(4, 5));
  EXPECT_EQ(&map.tileAt(4, 14), &map.tileAt(4, 2));
}

TEST(Map, WrapsAtTheFourCorners) {
  Map map(10, 6);

  EXPECT_EQ(&map.tileAt(-1, -1), &map.tileAt(9, 5));
  EXPECT_EQ(&map.tileAt(10, 6), &map.tileAt(0, 0));
  EXPECT_EQ(&map.tileAt(-1, 6), &map.tileAt(9, 0));
  EXPECT_EQ(&map.tileAt(10, -1), &map.tileAt(0, 5));
}

TEST(Map, TileContentIsConsultableThroughWrappedCoordinates) {
  Map map(10, 6);

  map.tileAt(0, 0).drop(ResourceType::Linemate, 2);

  EXPECT_EQ(map.tileAt(10, 6).quantityOf(ResourceType::Linemate), 2U);
  EXPECT_EQ(map.tileAt(-10, -6).quantityOf(ResourceType::Linemate), 2U);
}

TEST(Map, ConstLookupReadsTheSameContent) {
  Map map(4, 4);
  map.tileAt(2, 3).addPlayer(5);

  const Map &readOnly = map;

  EXPECT_EQ(readOnly.tileAt(2, 3).players().size(), 1U);
  EXPECT_EQ(readOnly.tileAt(6, 7).players().size(), 1U);
}
