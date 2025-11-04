#ifndef PATH_UTILS_H
#define PATH_UTILS_H
#include <filesystem>
#include <string>

void initBasePath();
std::string getResourcePath(const std::string &relativePath);

static std::string basePath;

inline void initBasePath() {
#ifdef __EMSCRIPTEN__
  basePath = "/";
#else
  basePath = std::filesystem::current_path().string();
#endif
}

inline std::string getResourcePath(const std::string &relativePath) {
  return (std::filesystem::path(basePath) / "resources" / relativePath)
      .string();
}

#endif // !PATH_UTILS_H
