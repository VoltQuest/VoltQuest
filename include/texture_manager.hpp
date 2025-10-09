#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include "raylib.h"
#include <string>
#include <unordered_map>

struct TextureManager {
  static std::unordered_map<std::string, Texture2D> &Registry();

  // Load SVG from file path (relative to resources directory)
  static void LoadSVG(const std::string &name, const std::string &filePath,
                      float scale = 1.0f);

  static Texture2D &Get(const std::string &name);
  static bool Exists(const std::string &name);
  static void UnloadAll();
};

#endif // TEXTURE_MANAGER_HPP
