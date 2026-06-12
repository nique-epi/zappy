/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for Observation (formatLook vision, formatInventory)
*/

#include <gtest/gtest.h>
#include <algorithm>
#include <string>
#include "App/World/Map/Map.hpp"
#include "App/World/Observation.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Inventory.hpp"
#include "App/World/Resources/ResourceType.hpp"

using zappy::world::Direction;
using zappy::world::formatInventory;
using zappy::world::formatLook;
using zappy::world::Inventory;
using zappy::world::Map;
using zappy::world::ResourceType;

namespace {

std::size_t countTiles(const std::string& look) {
  if (look == "[]") {
    return 0;
  }
  return 1 +
         static_cast<std::size_t>(std::count(look.begin(), look.end(), ','));
}

}  // namespace

TEST(Observation, LookSeesOnlyTheCurrentTileAtLevelOneOnEmptyWorld) {
  /*
   * Given a level-1 drone on an empty world with nothing around it
   * When it looks
   * Then it reports four empty tiles (its own plus the row ahead)
   */
  Map map(7, 7);

  const std::string look = formatLook(3, 3, Direction::North, 1, map);

  EXPECT_EQ(look, "[,,,]");
  EXPECT_EQ(countTiles(look), 4U);
}

TEST(Observation, LookCountsGrowAsTheSquareOfLevelPlusOne) {
  /*
   * Given drones of increasing level on an empty world
   * When each looks
   * Then the number of visible tiles is (level + 1) squared
   */
  Map map(15, 15);

  EXPECT_EQ(countTiles(formatLook(7, 7, Direction::North, 1, map)), 4U);
  EXPECT_EQ(countTiles(formatLook(7, 7, Direction::North, 2, map)), 9U);
  EXPECT_EQ(countTiles(formatLook(7, 7, Direction::North, 3, map)), 16U);
}

TEST(Observation, LookNumbersTilesFromCurrentThenLeftToRightAhead) {
  /*
   * Given a north-facing level-1 drone with a distinct resource on each tile
   *       of the row ahead (left, centre, right)
   * When it looks
   * Then the tiles read current first, then left to right
   */
  Map map(7, 7);
  map.tileAt(2, 2).drop(ResourceType::Sibur);
  map.tileAt(3, 2).drop(ResourceType::Linemate);
  map.tileAt(4, 2).drop(ResourceType::Mendiane);

  const std::string look = formatLook(3, 3, Direction::North, 1, map);

  EXPECT_EQ(look, "[,sibur,linemate,mendiane]");
}

TEST(Observation, LookListsPlayersBeforeResourcesAndRepeatsUnits) {
  /*
   * Given the current tile holding two players and two food units
   * When a level-1 drone looks
   * Then tile zero lists both players then both food units, space separated
   */
  Map map(7, 7);
  map.tileAt(3, 3).addPlayer(1);
  map.tileAt(3, 3).addPlayer(2);
  map.tileAt(3, 3).drop(ResourceType::Food, 2);

  const std::string look = formatLook(3, 3, Direction::North, 1, map);

  EXPECT_EQ(look.rfind("[player player food food,", 0), 0U);
}

TEST(Observation, LookWrapsToroidallyAcrossEdges) {
  /*
   * Given a north-facing level-1 drone on the top-left corner of a 3x3 world
   *       with a marker placed where the forward-left tile wraps to
   * When it looks
   * Then the wrapped tile carries the marker at its numbered slot
   */
  Map map(3, 3);
  map.tileAt(2, 2).drop(ResourceType::Thystame);

  const std::string look = formatLook(0, 0, Direction::North, 1, map);

  EXPECT_EQ(look, "[,thystame,,]");
}

TEST(Observation, LookReadsLeftToRightRelativeToFacing) {
  /*
   * Given an east-facing level-1 drone with a marker on its forward-left tile
   * When it looks
   * Then the marker lands on the first tile of the row ahead (the left one)
   */
  Map map(7, 7);
  map.tileAt(4, 2).drop(ResourceType::Linemate);

  const std::string look = formatLook(3, 3, Direction::East, 1, map);

  EXPECT_EQ(look, "[,linemate,,]");
}

TEST(Observation, InventoryListsTheSevenResourcesInProtocolOrder) {
  /*
   * Given a fresh inventory charged with the initial ten food units
   * When it is serialised
   * Then the seven kinds appear in protocol order with their quantities
   */
  Inventory inventory;
  inventory.add(ResourceType::Food, 10);

  EXPECT_EQ(formatInventory(inventory),
            "[food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, "
            "thystame 0]");
}

TEST(Observation, InventoryReflectsEveryStockedKind) {
  /*
   * Given an inventory carrying several kinds at distinct quantities
   * When it is serialised
   * Then each kind reports its own count
   */
  Inventory inventory;
  inventory.add(ResourceType::Food, 3);
  inventory.add(ResourceType::Sibur, 5);
  inventory.add(ResourceType::Thystame, 2);

  EXPECT_EQ(formatInventory(inventory),
            "[food 3, linemate 0, deraumere 0, sibur 5, mendiane 0, phiras 0, "
            "thystame 2]");
}
