#include "../include/window_manager.hpp"
#include "../include/settings.hpp"
#include "raylib.h"
#include <iostream>

void createWindow() {

  if (globalSettings.fullscreen) {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
  } else {
    SetConfigFlags(FLAG_BORDERLESS_WINDOWED_MODE);
  }
  InitWindow(0, 0, "VoltQuest");
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();

  if (overrideSettings()) {
    globalSettings.screenWidth = screenWidth;
    globalSettings.screenHeight = screenHeight;
    globalSettings.refreshRate = 60;
    globalSettings.fullscreen = true;
    saveSettings();
  }

  // Create a the game window
  SetWindowSize(globalSettings.screenWidth, globalSettings.screenHeight);

  SetExitKey(0); // Disables Escape key from CloseWindow
  SetTargetFPS(globalSettings.refreshRate);
}
