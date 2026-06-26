/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SkyRenderer
*/

#include "Render/SkyRenderer.hpp"
#include <raylib.h>
#include <rlgl.h>
#include <string>

namespace zappy::gui {

namespace {

constexpr float skyScale = 500.0F;
constexpr float hdrExposure = 1.5F;
constexpr int rgbChannels = 3;

Texture2D skyTex_{};
Model skyModel_{};

std::string skyPath() { return std::string(ASSETS_DIR) + "/menu/sky.hdr"; }

void applyReinhard(Image& hdr) {
  if (hdr.format != PIXELFORMAT_UNCOMPRESSED_R32G32B32) {
    return;
  }
  auto* pixels = static_cast<float*>(hdr.data);
  const int count = hdr.width * hdr.height * rgbChannels;
  for (int i = 0; i < count; ++i) {
    const float v = pixels[i] * hdrExposure;
    pixels[i] = v / (v + 1.0F);
  }
}

}  // namespace

void SkyRenderer::load() {
  if (skyTex_.id > 0) {
    return;
  }

  Image hdr = LoadImage(skyPath().c_str());
  if (hdr.data == nullptr) {
    return;
  }

  applyReinhard(hdr);
  ImageFormat(&hdr, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  skyTex_ = LoadTextureFromImage(hdr);
  SetTextureFilter(skyTex_, TEXTURE_FILTER_BILINEAR);
  UnloadImage(hdr);

  if (skyTex_.id == 0) {
    return;
  }

  skyModel_ = LoadModelFromMesh(GenMeshSphere(1.0F, 32, 32));
  skyModel_.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTex_;
}

void SkyRenderer::unload() {
  if (skyTex_.id == 0) {
    return;
  }
  UnloadModel(skyModel_);
  UnloadTexture(skyTex_);
  skyTex_ = {};
  skyModel_ = {};
}

void SkyRenderer::draw(Camera3D camera) {
  if (skyTex_.id == 0) {
    return;
  }
  rlDisableBackfaceCulling();
  rlDisableDepthMask();
  DrawModel(skyModel_, camera.position, skyScale, WHITE);
  rlEnableDepthMask();
  rlEnableBackfaceCulling();
}

}  // namespace zappy::gui
