#include "../include/settings.hpp"
#include "../include/path_utils.hpp"
#include "../include/ui_manager.hpp"
#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <direct.h> // for _mkdir
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

static bool overrideDisplaySettings = true;
std::string settingsPath;

// Helper: trim whitespace
std::string trim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");
  return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

// Helper: create folder if it doesn't exist
void createFolderIfMissing(const std::string &folder) {
#ifdef _WIN32
  _mkdir(folder.c_str());
#else
  mkdir(folder.c_str(), 0755);
#endif
}

// Setup the OS-specific settings path
void initSettingsPath() {
#ifdef _WIN32
  const char *appdata = getenv("APPDATA");
  std::string folder = std::string(appdata) + "\\VoltQuest\\";
#elif defined(__APPLE__) && defined(__MACH__)
  const char *home = getenv("HOME");
  std::string folder =
      std::string(home) + "/Library/Application Support/VoltQuest/";
#elif defined(__linux__)
  const char *xdg = getenv("XDG_CONFIG_HOME");
  if (!xdg)
    xdg = getenv("HOME");
  std::string folder = std::string(xdg) + "/.config/VoltQuest/";
#else
#error "Unknown OS"
#endif

  createFolderIfMissing(folder);
  settingsPath = folder + "settings.cfg";
}

// Save settings
void saveSettings() {
  std::ofstream settingsFile{settingsPath, std::ios::out};
  settingsFile << "# VoltQuest Settings\n";
  settingsFile
      << "# Safe to edit. The game will regenerate this file if deleted.\n\n";

  settingsFile << "ScreenWidth = " << globalSettings.screenWidth << '\n';
  settingsFile << "ScreenHeight = " << globalSettings.screenHeight << '\n';
  settingsFile << "RefreshRate = " << globalSettings.refreshRate << '\n';
  settingsFile << "FullScreen = "
               << (globalSettings.fullscreen ? "true" : "false") << '\n';
}

// Load settings
void loadSettings() {
  std::ifstream settingsFile{settingsPath, std::ios::in};
  overrideDisplaySettings = !settingsFile.good();

  std::string line;
  while (std::getline(settingsFile, line)) {
    line = trim(line);
    if (line.empty() || line[0] == '#' || line[0] == ';')
      continue;

    size_t equalPos = line.find('=');
    if (equalPos == std::string::npos)
      continue;

    std::string key = trim(line.substr(0, equalPos));
    std::string value = trim(line.substr(equalPos + 1));

    if (key == "ScreenWidth") {
      globalSettings.screenWidth = std::stoi(value);
      overrideDisplaySettings = false;
    }
    if (key == "ScreenHeight") {
      globalSettings.screenHeight = std::stoi(value);
      overrideDisplaySettings = false;
    }
    if (key == "RefreshRate") {
      globalSettings.refreshRate = std::stoi(value);
      overrideDisplaySettings = false;
    }
    if (key == "FullScreen") {
      globalSettings.fullscreen = (value == "true" || value == "1");
      overrideDisplaySettings = false;
    }
  }
}

bool overrideSettings() { return overrideDisplaySettings; }
