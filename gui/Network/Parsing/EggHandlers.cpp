/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EggHandlers — handlers for egg-related server messages (enw, ebo, edi)
*/

#include <sstream>
#include "Network/Parsing/HandlerTypes.hpp"
#include "Network/Parsing/Handlers.hpp"
#include "Network/Parsing/ParsingUtils.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

void registerEggHandlers(HandlerMap& handlers, WorldState& world) {
  handlers["enw"] = [&world](std::istringstream& stream) {
    const int eggId =  // NOLINT(cppcoreguidelines-init-variables)
        parseId(stream);
    const int playerId =  // NOLINT(cppcoreguidelines-init-variables)
        parseId(stream);
    int posX = 0;
    int posY = 0;
    stream >> posX >> posY;
    world.eggs.push_back(
        Egg{.id = eggId, .playerId = playerId, .x = posX, .y = posY});
  };

  const auto removeEgg = [&world](std::istringstream& stream) {
    const int eggId =  // NOLINT(cppcoreguidelines-init-variables)
        parseId(stream);
    std::erase_if(world.eggs,
                  [eggId](const Egg& egg) { return egg.id == eggId; });
  };
  handlers["ebo"] = handlers["edi"] = removeEgg;
}

}  // namespace zappy::gui
