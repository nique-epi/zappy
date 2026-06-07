/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceType — the resource kinds that can lie on the world floor
*/

#include "App/World/Resources/ResourceType.hpp"

namespace zappy::world {

const std::array<ResourceType, resourceTypeCount> &allResourceTypes() {
  static const std::array<ResourceType, resourceTypeCount> types{
      ResourceType::Food,    ResourceType::Linemate, ResourceType::Deraumere,
      ResourceType::Sibur,   ResourceType::Mendiane, ResourceType::Phiras,
      ResourceType::Thystame};
  return types;
}

std::string_view resourceName(ResourceType type) {
  switch (type) {
    case ResourceType::Food:
      return "food";
    case ResourceType::Linemate:
      return "linemate";
    case ResourceType::Deraumere:
      return "deraumere";
    case ResourceType::Sibur:
      return "sibur";
    case ResourceType::Mendiane:
      return "mendiane";
    case ResourceType::Phiras:
      return "phiras";
    case ResourceType::Thystame:
      return "thystame";
  }
  return "";
}

}  // namespace zappy::world
