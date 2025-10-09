#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "../include/texture_manager.hpp"
#include "cstdint"
#include "nanosvg.h"
#include "nanosvgrast.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

std::unordered_map<std::string, Texture2D> &TextureManager::Registry() {
  static std::unordered_map<std::string, Texture2D> textures;
  return textures;
}

void TextureManager::LoadSVG(const std::string &name,
                             const std::string &filePath, float scale) {
  // Validate inputs
  if (filePath.empty()) {
    return;
  }
  if (scale == 0.0f) {
    scale = 1.0f;
  }

  // Get full path using path_utils

  // Read SVG file
  std::ifstream file(filePath);
  if (!file.is_open()) {
    printf("Failed to open SVG file: %s\n", filePath.c_str());
    return;
  }

  // Read file content into string
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  std::string svgContent = buffer.str();
  if (svgContent.empty()) {
    printf("SVG file is empty: %s\n", filePath.c_str());
    return;
  }

  // Create a mutable copy of the SVG data since nsvgParse modifies it
  size_t svgLen = svgContent.length();
  char *svgCopy = (char *)malloc(svgLen + 1);
  if (!svgCopy) {
    printf("Failed to allocate memory for SVG copy\n");
    return;
  }
  strcpy(svgCopy, svgContent.c_str());

  NSVGimage *svg = nsvgParse(svgCopy, "px", 96.0f);
  free(svgCopy); // Free the copy immediately after parsing

  if (!svg) {
    printf("Failed to parse SVG: %s\n", filePath.c_str());
    return;
  }

  // Validate SVG dimensions
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

  // Calculate final dimensions
  int w = (int)(svg->width * scale + 0.5f); // Round to nearest int
  int h = (int)(svg->height * scale + 0.5f);

  // Ensure minimum size
  if (w < 1)
    w = 1;
  if (h < 1)
    h = 1;

  // Calculate buffer size with safety check
  size_t pixelCount = (size_t)w * (size_t)h;
  size_t bufSize = pixelCount * 4; // 4 bytes per pixel (RGBA)
  if (bufSize == 0 || pixelCount > (SIZE_MAX / 4)) {
    printf("Invalid buffer size calculation\n");
    nsvgDelete(svg);
    nsvgDeleteRasterizer(rast);
    return;
  }

  unsigned char *imgBuffer =
      (unsigned char *)calloc(bufSize, 1); // Use calloc for zero-init
  if (!imgBuffer) {
    printf("Failed to allocate image buffer\n");
    nsvgDelete(svg);
    nsvgDeleteRasterizer(rast);
    return;
  }

  // Rasterize the SVG
  nsvgRasterize(rast, svg, 0, 0, scale, imgBuffer, w, h, w * 4);

  // Create Raylib image
  Image rlImage = {0};
  rlImage.data = imgBuffer;
  rlImage.width = w;
  rlImage.height = h;
  rlImage.mipmaps = 1;
  rlImage.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

  Texture2D tex = LoadTextureFromImage(rlImage);
  if (tex.id == 0) {
    printf("Failed to create texture from image\n");
    free(imgBuffer);
    nsvgDelete(svg);
    nsvgDeleteRasterizer(rast);
    return;
  }

  // Store in registry
  Registry()[name] = tex;
  printf("Successfully loaded SVG texture '%s' from %s (%dx%d)\n", name.c_str(),
         filePath.c_str(), w, h);

  // Clean up
  free(imgBuffer);
  nsvgDelete(svg);
  nsvgDeleteRasterizer(rast);
}

Texture2D &TextureManager::Get(const std::string &name) {
  auto it = Registry().find(name);
  if (it == Registry().end()) {
    static Texture2D emptyTexture = {0};
    return emptyTexture;
  }
  return it->second;
}

bool TextureManager::Exists(const std::string &name) {
  return Registry().find(name) != Registry().end();
}

void TextureManager::UnloadAll() {
  for (auto &[name, tex] : Registry()) {
    if (tex.id != 0) {
      UnloadTexture(tex);
    }
  }
  Registry().clear();
}
