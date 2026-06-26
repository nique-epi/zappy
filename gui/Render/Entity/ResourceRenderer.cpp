/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceRenderer
*/

#include "Render/Entity/ResourceRenderer.hpp"
#include <raylib.h>
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>
#include "Render/Entity/ResourceRendererConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr const char* colormapPath =
    ASSETS_DIR "/models/resources/food/colormap.png";

constexpr std::array<const char*, 18> foodModelPaths{{
    ASSETS_DIR "/models/resources/food/apple.glb",
    ASSETS_DIR "/models/resources/food/banana.glb",
    ASSETS_DIR "/models/resources/food/bread.glb",
    ASSETS_DIR "/models/resources/food/broccoli.glb",
    ASSETS_DIR "/models/resources/food/burger.glb",
    ASSETS_DIR "/models/resources/food/carrot.glb",
    ASSETS_DIR "/models/resources/food/cherries.glb",
    ASSETS_DIR "/models/resources/food/cheese.glb",
    ASSETS_DIR "/models/resources/food/corn.glb",
    ASSETS_DIR "/models/resources/food/donut.glb",
    ASSETS_DIR "/models/resources/food/grapes.glb",
    ASSETS_DIR "/models/resources/food/lemon.glb",
    ASSETS_DIR "/models/resources/food/mushroom.glb",
    ASSETS_DIR "/models/resources/food/orange.glb",
    ASSETS_DIR "/models/resources/food/pizza.glb",
    ASSETS_DIR "/models/resources/food/strawberry.glb",
    ASSETS_DIR "/models/resources/food/tomato.glb",
    ASSETS_DIR "/models/resources/food/donut.glb",
}};

constexpr std::size_t foodModelCount = foodModelPaths.size();

constexpr std::size_t gemCount = RESOURCE_COUNT - 1;
constexpr std::array<const char*, gemCount> gemModelPaths{{
    ASSETS_DIR "/models/resources/gems/Garnet.obj",
    ASSETS_DIR "/models/resources/gems/Sapphire.obj",
    ASSETS_DIR "/models/resources/gems/Topaz.obj",
    ASSETS_DIR "/models/resources/gems/Amethyst.obj",
    ASSETS_DIR "/models/resources/gems/Emerald.obj",
    ASSETS_DIR "/models/resources/gems/Diamond.obj",
}};

std::size_t foodIndexForTile(int tileX, int tileY) {
  const auto hash = (static_cast<unsigned>(tileX) * 31U +
                     static_cast<unsigned>(tileY) * 97U) &
                    0x7FFFFFFFU;
  return hash % foodModelCount;
}

void drawAtSlot(Model& model, cfg::SlotDrawConfig drawCfg) {
  const cfg::ResourceLayoutSlot& slot = cfg::RESOURCE_LAYOUT[drawCfg.slotIndex];
  const auto time = static_cast<float>(GetTime());
  const float bobY = sinf((time * cfg::RESOURCE_BOB_SPEED) + slot.phase) *
                     cfg::RESOURCE_BOB_AMPLITUDE;
  const float rotationDeg =
      fmodf((time * cfg::RESOURCE_ROTATION_SPEED) +
                (static_cast<float>(drawCfg.slotIndex) *
                 (360.0F / static_cast<float>(RESOURCE_COUNT))),
            360.0F);
  const Vector3 position{drawCfg.worldPos.x + slot.offsetX,
                         cfg::RESOURCE_BASE_Y + bobY,
                         drawCfg.worldPos.z + slot.offsetZ};
  DrawModelEx(model, position, {0.0F, 1.0F, 0.0F}, rotationDeg,
              {drawCfg.scale, drawCfg.scale, drawCfg.scale}, WHITE);
}

Texture2D loadColormap() {
  const Texture2D colormap = LoadTexture(colormapPath);
  if (colormap.id == 0) {
    std::cerr << "[ResourceRenderer] failed to load colormap: " << colormapPath
              << '\n';
  }
  return colormap;
}

std::vector<Model> loadFoodModels(const Texture2D& colormap) {
  std::vector<Model> models;
  models.reserve(foodModelCount);
  for (std::size_t i = 0; i < foodModelCount; ++i) {
    Model model = LoadModel(foodModelPaths[i]);
    if (model.meshCount == 0) {
      std::cerr << "[ResourceRenderer] failed to load " << foodModelPaths[i]
                << '\n';
    } else {
      for (int matIdx = 0; matIdx < model.materialCount; ++matIdx) {
        model.materials[matIdx].maps[MATERIAL_MAP_DIFFUSE].texture = colormap;
      }
    }
    models.push_back(model);
  }
  return models;
}

std::vector<Model> loadGemModels() {
  std::vector<Model> models;
  models.reserve(gemCount);
  for (std::size_t i = 0; i < gemCount; ++i) {
    const Model model = LoadModel(gemModelPaths[i]);
    if (model.meshCount == 0) {
      std::cerr << "[ResourceRenderer] failed to load " << gemModelPaths[i]
                << '\n';
    }
    models.push_back(model);
  }
  return models;
}

void drawTileFood(std::vector<Model>& foodModels, const Tile& tile, int tileX,
                  int tileY, cfg::TileWorldPos pos) {
  if (tile.resources[0] == 0) {
    return;
  }
  const std::size_t foodIdx = foodIndexForTile(tileX, tileY);
  drawAtSlot(
      foodModels[foodIdx],
      {.slotIndex = 0, .scale = cfg::RESOURCE_FOOD_SCALE, .worldPos = pos});
}

void drawTileGems(std::vector<Model>& gemModels, const Tile& tile,
                  cfg::TileWorldPos pos) {
  for (std::size_t i = 1; i < RESOURCE_COUNT; ++i) {
    if (tile.resources[i] > 0) {
      drawAtSlot(
          gemModels[i - 1],
          {.slotIndex = i, .scale = cfg::RESOURCE_GEM_SCALE, .worldPos = pos});
    }
  }
}

void unloadModels(std::vector<Model>& models, bool clearDiffuseTexture) {
  for (auto& model : models) {
    if (clearDiffuseTexture) {
      for (int matIdx = 0; matIdx < model.materialCount; ++matIdx) {
        model.materials[matIdx].maps[MATERIAL_MAP_DIFFUSE].texture = {};
      }
    }
    UnloadModel(model);
  }
  models.clear();
}

}  // namespace

void ResourceRenderer::loadAssets() {
  colormap_ = loadColormap();
  foodModels_ = loadFoodModels(colormap_);
  gemModels_ = loadGemModels();
}

void ResourceRenderer::draw(WorldState& world) {
  for (int tileY = 0; tileY < world.height; ++tileY) {
    for (int tileX = 0; tileX < world.width; ++tileX) {
      const Tile& tile = world.tiles[tileY][tileX];
      const cfg::TileWorldPos pos{.x = tileToWorld(tileX),
                                  .z = tileToWorld(tileY)};
      drawTileFood(foodModels_, tile, tileX, tileY, pos);
      drawTileGems(gemModels_, tile, pos);
    }
  }
}

void ResourceRenderer::drawOverlay([[maybe_unused]] const WorldState& world,
                                   [[maybe_unused]] const Camera3D& camera) {}

void ResourceRenderer::unloadAssets() {
  unloadModels(foodModels_, true);
  UnloadTexture(colormap_);
  unloadModels(gemModels_, false);
}

}  // namespace zappy::gui
