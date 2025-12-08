#include "../include/window_manager.hpp"
#include "../include/settings.hpp"
#include "raylib.h"

void createWindow() {
#ifndef EMSCRIPTEN
  SetConfigFlags(FLAG_VSYNC_HINT);
  SetConfigFlags(FLAG_MSAA_4X_HINT);
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
  if (globalSettings.fullscreen) {
    SetWindowState(FLAG_FULLSCREEN_MODE);
  } else {
    SetWindowState(FLAG_WINDOW_RESIZABLE);
  }

  // Create a the game window
  SetWindowSize(globalSettings.screenWidth, globalSettings.screenHeight);

#else
  globalSettings.screenWidth = 960;
  globalSettings.screenHeight = 540;
  globalSettings.fullscreen = false;
  globalSettings.refreshRate = 60;
  InitWindow(globalSettings.screenWidth, globalSettings.screenHeight,
             "VoltQuest");
#endif

  SetExitKey(0); // Disables Escape key from CloseWindow
  SetTargetFPS(globalSettings.refreshRate);
}
