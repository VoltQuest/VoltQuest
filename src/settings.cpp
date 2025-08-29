#include "../include/settings.hpp"
#include "../include/path_utils.hpp"
#include "../include/ui_manager.hpp"
#include <fstream>
#include <iostream>
#include <string>

static bool overrideDisplaySettings = true;
std::string settingsPath = "./resources/settings.cfg";

void saveSettings() {
  std::ofstream settingsFile{settingsPath, std::ios::out};
  settingsFile << "# VoltQuest Settings\n";
  settingsFile
      << "# Safe to edit. The game will regenerate this file if deleted.\n";
  settingsFile << '\n';

  settingsFile << "ScreenWidth = " << globalSettings.screenWidth << '\n';
  settingsFile << "ScreenHeight = " << globalSettings.screenHeight << '\n';
  settingsFile << "RefreshRate = " << globalSettings.refreshRate << '\n';
  settingsFile << "FullScreen = "
               << (globalSettings.fullscreen ? "true" : "false") << '\n';
}

std::string trim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");
  return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

void loadSettings() {
  std::ifstream settingsFile{settingsPath, std::ios::in};
  std::string line;
  overrideDisplaySettings = (settingsFile) ? false : true;
  while (std::getline(settingsFile, line)) {
    line = trim(line);
    if (line.empty() || line[0] == '#' || line[0] == ';')
      continue;

    size_t equalPos = line.find('=');

    if (equalPos == std::string::npos)
      continue;

    std::string key = trim(line.substr(0, equalPos));
    std::string value = trim(line.substr(equalPos + 1));

    // Map keys to globalSettings
    std::cout << key << ":" << value << std::endl;
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
