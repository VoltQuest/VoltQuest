#ifndef PATH_UTILS_H
#define PATH_UTILS_H
#include <string>

inline std::string getResourcePath(const std::string &relativePath) {
  return "./resources/" + relativePath;
}

#endif // !PATH_UTILS_H
