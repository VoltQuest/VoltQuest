#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>

struct settings {
  bool isGameRunning = true;
  int currentMonitor = 0;
  int monitorWidth = 1920;
  int monitorHeight = 1080;
  int screenWidth = 1920;
  int screenHeight = 1080;
  int refreshRate = 60;
  bool fullscreen = false;
};

inline settings globalSettings;
void initSettingsPath();
void saveSettings();
std::string trim(const std::string &s);
void loadSettings();
bool overrideSettings();
#endif
