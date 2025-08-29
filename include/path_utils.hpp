#ifndef PATH_UTILS_H
#define PATH_UTILS_H
#include <filesystem>
#include <string>

void initBasePath();
std::string getResourcePath(const std::string &relativePath);

static std::string basePath;

inline void initBasePath() {
  basePath = std::filesystem::current_path().string();
}

inline std::string getResourcePath(const std::string &relativePath) {
  return basePath + "resources/" + relativePath;
}

#endif // !PATH_UTILS_H
