#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION

#include "../include/texture_manager.hpp"
#include "nanosvg.h"
#include "nanosvgrast.h"

#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>

// File-local texture store.
// Textures live for the lifetime of the program and are owned by this TU.
static std::unordered_map<std::string, Texture2D> textures;

void TextureManager::LoadSVG(const std::string &name,
                             const std::string &filePath, float scale) {
  // Prevent accidental double-loads
  if (Exists(name)) {
    printf("ERR:%s Texture already exists\n", name.c_str());
    return;
  }

  if (filePath.empty())
    return;
  if (scale == 0.0f)
    scale = 1.0f;

  std::ifstream file(filePath);
  if (!file.is_open()) {
    printf("Failed to open SVG file: %s\n", filePath.c_str());
    return;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string svgContent = buffer.str();

  if (svgContent.empty()) {
    printf("SVG file is empty: %s\n", filePath.c_str());
    return;
  }

  // NanoSVG mutates the input buffer, so we must provide a writable copy
  std::vector<char> svgCopy(svgContent.begin(), svgContent.end());
  svgCopy.push_back('\0');

  NSVGimage *svg = nsvgParse(svgCopy.data(), "px", 96.0f);
  if (!svg) {
    printf("Failed to parse SVG: %s\n", filePath.c_str());
    return;
  }

  // Reject malformed SVGs early
  if (svg->width <= 0 || svg->height <= 0) {
    printf("Invalid SVG dimensions: %fx%f\n", svg->width, svg->height);
    nsvgDelete(svg);
    return;
  }

  NSVGrasterizer *rast = nsvgCreateRasterizer();
  if (!rast) {
    printf("Failed to create SVG rasterizer\n");
    nsvgDelete(svg);
    return;
  }

  // Final raster size (rounded, clamped)
  int w = (int)(svg->width * scale + 0.5f);
  int h = (int)(svg->height * scale + 0.5f);
  if (w < 1)
    w = 1;
  if (h < 1)
    h = 1;

  size_t pixelCount = (size_t)w * (size_t)h;
  if (pixelCount > SIZE_MAX / 4) {
    printf("Invalid buffer size calculation\n");
    nsvgDelete(svg);
    nsvgDeleteRasterizer(rast);
    return;
  }

  // CPU-side RGBA buffer for rasterization
  std::vector<unsigned char> imgBuffer(pixelCount * 4, 0);

  nsvgRasterize(rast, svg, 0, 0, scale, imgBuffer.data(), w, h, w * 4);

  Image rlImage = {};
  rlImage.data = imgBuffer.data();
  rlImage.width = w;
  rlImage.height = h;
  rlImage.mipmaps = 1;
  rlImage.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

  // Upload to GPU; after this, imgBuffer is no longer needed
  Texture2D tex = LoadTextureFromImage(rlImage);
  if (tex.id == 0) {
    printf("Failed to create texture from image\n");
    nsvgDelete(svg);
    nsvgDeleteRasterizer(rast);
    return;
  }

  textures[name] = tex;

  printf("Successfully loaded SVG texture '%s' from %s (%dx%d)\n", name.c_str(),
         filePath.c_str(), w, h);

  nsvgDelete(svg);
  nsvgDeleteRasterizer(rast);
}

Texture2D &TextureManager::Get(const std::string &name) {
  auto it = textures.find(name);
  if (it == textures.end()) {
    // Safe null texture to avoid crashes at call sites
    static Texture2D empty = {0};
    return empty;
  }
  return it->second;
}

bool TextureManager::Exists(const std::string &name) {
  return textures.find(name) != textures.end();
}

void TextureManager::UnloadAll() {
  // Explicitly free GPU resources
  for (auto &[name, tex] : textures) {
    if (tex.id != 0) {
      UnloadTexture(tex);
    }
  }
  textures.clear();
}
