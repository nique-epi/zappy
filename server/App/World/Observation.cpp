/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Observation — textual views a drone reports: its surroundings and its stock
*/

#include "App/World/Observation.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Inventory.hpp"
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Tile/Tile.hpp"

namespace zappy::world {

namespace {

void appendObject(std::string& objects, std::string_view name) {
  if (!objects.empty()) {
    objects += ' ';
  }
  objects.append(name);
}

std::string formatTileObjects(const Tile& tile) {
  std::string objects;
  for (std::size_t player = 0; player < tile.players().size(); player++) {
    appendObject(objects, "player");
  }
  for (const ResourceType type : allResourceTypes()) {
    const std::size_t quantity = tile.quantityOf(type);
    for (std::size_t unit = 0; unit < quantity; unit++) {
      appendObject(objects, resourceName(type));
    }
  }
  return objects;
}

}  // namespace

std::string formatLook(int column, int row, Direction facing, int level,
                       const Map& map) {
  const DirectionOffset forward = directionOffset(facing);
  const DirectionOffset rightward = {.dx = -forward.dy, .dy = forward.dx};
  std::string content = "[";
  bool first = true;
  for (int distance = 0; distance <= level; distance++) {
    for (int lateral = -distance; lateral <= distance; lateral++) {
      const int tileColumn =
          column + (forward.dx * distance) + (rightward.dx * lateral);
      const int tileRow =
          row + (forward.dy * distance) + (rightward.dy * lateral);
      if (!first) {
        content += ',';
      }
      first = false;
      content += formatTileObjects(map.tileAt(tileColumn, tileRow));
    }
  }
  content += ']';
  return content;
}

std::string formatInventory(const Inventory& inventory) {
  std::string content = "[";
  bool first = true;
  for (const ResourceType type : allResourceTypes()) {
    if (!first) {
      content += ", ";
    }
    first = false;
    content.append(resourceName(type));
    content += ' ';
    content += std::to_string(inventory.quantityOf(type));
  }
  content += ']';
  return content;
}

}  // namespace zappy::world
