/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EntityRendererFactory — instantiates the map entity renderers
*/

#include "Render/Entity/EntityRendererFactory.hpp"
#include <array>
#include <memory>
#include <vector>
#include "Render/Entity/EggRenderer.hpp"
#include "Render/Entity/IEntityRenderer.hpp"
#include "Render/Entity/IncantationRenderer.hpp"
#include "Render/Entity/PlayerRenderer.hpp"
#include "Render/Entity/ResourceRenderer.hpp"

namespace zappy::gui {

std::unique_ptr<IEntityRenderer> EntityRendererFactory::create(
    EntityKind kind) {
  switch (kind) {
    case EntityKind::Resource:
      return std::make_unique<ResourceRenderer>();
    case EntityKind::Egg:
      return std::make_unique<EggRenderer>();
    case EntityKind::Player:
      return std::make_unique<PlayerRenderer>();
    case EntityKind::Incantation:
      return std::make_unique<IncantationRenderer>();
  }
  return nullptr;
}

std::vector<std::unique_ptr<IEntityRenderer>>
EntityRendererFactory::createAll() {
  constexpr std::array drawOrder{EntityKind::Resource, EntityKind::Egg,
                                 EntityKind::Player, EntityKind::Incantation};
  std::vector<std::unique_ptr<IEntityRenderer>> renderers;
  renderers.reserve(drawOrder.size());
  for (const EntityKind kind : drawOrder) {
    renderers.push_back(create(kind));
  }
  return renderers;
}

}  // namespace zappy::gui
