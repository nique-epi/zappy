/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceRendererConfig
*/

#pragma once

#include <array>

namespace zappy::gui::config {

constexpr float RESOURCE_FOOD_SCALE = 0.80F;
constexpr float RESOURCE_GEM_SCALE = 0.267F;
constexpr float RESOURCE_BASE_Y = 0.05F;
constexpr float RESOURCE_BOB_SPEED = 1.8F;
constexpr float RESOURCE_BOB_AMPLITUDE = 0.025F;
constexpr float RESOURCE_ROTATION_SPEED = 22.0F;

struct ResourceLayoutSlot {
  float offsetX;
  float offsetZ;
  float phase;
};

constexpr std::array<ResourceLayoutSlot, 7> RESOURCE_LAYOUT{{
    {.offsetX = -0.28F, .offsetZ = -0.28F, .phase = 0.000F},
    {.offsetX = 0.00F, .offsetZ = -0.28F, .phase = 0.898F},
    {.offsetX = 0.28F, .offsetZ = -0.28F, .phase = 1.795F},
    {.offsetX = -0.28F, .offsetZ = 0.00F, .phase = 2.693F},
    {.offsetX = 0.28F, .offsetZ = 0.00F, .phase = 3.590F},
    {.offsetX = -0.14F, .offsetZ = 0.28F, .phase = 4.488F},
    {.offsetX = 0.14F, .offsetZ = 0.28F, .phase = 5.386F},
}};

}  // namespace zappy::gui::config
