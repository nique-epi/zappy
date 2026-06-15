/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceType — the resource kinds that can lie on the world floor
*/

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

namespace zappy::world {

/**
 * @brief The kinds of resource a tile can hold: food plus the six minerals.
 *
 * The order matches the GUI protocol fields `q0..q6` (food first, then the six
 * minerals), so the underlying index can be written on the wire directly. Only
 * the kind of a unit is identifiable: two units of the same kind are
 * indistinguishable, hence counts rather than individual objects.
 */
enum class ResourceType : std::uint8_t {
  Food,
  Linemate,
  Deraumere,
  Sibur,
  Mendiane,
  Phiras,
  Thystame,
};

/// Number of distinct resource kinds; also the width of a per-tile count array.
constexpr std::size_t resourceTypeCount = 7;

/**
 * @brief Every resource kind in protocol order (food, then the six minerals).
 * @returns A stable array that callers iterate to walk all kinds without
 *          hard-coding the individual names.
 */
const std::array<ResourceType, resourceTypeCount>& allResourceTypes();

/**
 * @brief Canonical lower-case name of a resource kind.
 * @param[in] type The resource kind to name.
 * @returns The token used by the AI and GUI protocols (e.g. "linemate").
 */
std::string_view resourceName(ResourceType type);

/**
 * @brief Resolve the resource kind carried by a protocol token.
 * @param[in] name Lower-case token as sent by the AI (e.g. "linemate").
 * @returns The matching kind, or std::nullopt when @p name names no resource.
 */
[[nodiscard]] std::optional<ResourceType> resourceFromName(
    std::string_view name);

}  // namespace zappy::world
