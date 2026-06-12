/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceType — the resource kinds that can lie on the world floor
*/

#include "App/World/Resources/ResourceType.hpp"

namespace zappy::world {

const std::array<ResourceType, resourceTypeCount>& allResourceTypes() {
  static const std::array<ResourceType, resourceTypeCount> types{
      ResourceType::Food,    ResourceType::Linemate, ResourceType::Deraumere,
      ResourceType::Sibur,   ResourceType::Mendiane, ResourceType::Phiras,
      ResourceType::Thystame};
  return types;
}

std::string_view resourceName(ResourceType type) {
  static constexpr std::array names{
      std::string_view("food"),      std::string_view("linemate"),
      std::string_view("deraumere"), std::string_view("sibur"),
      std::string_view("mendiane"),  std::string_view("phiras"),
      std::string_view("thystame")};
  static_assert(names.size() == resourceTypeCount,
                "resourceName table out of sync with ResourceType");

  return names[static_cast<std::size_t>(type)];
}

std::optional<ResourceType> resourceFromName(std::string_view name) {
  for (const ResourceType type : allResourceTypes()) {
    if (resourceName(type) == name) {
      return type;
    }
  }
  return std::nullopt;
}

}  // namespace zappy::world
