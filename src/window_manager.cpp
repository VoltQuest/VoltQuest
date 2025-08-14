#include "../include/window_manager.hpp"
#include "../include/settings.hpp"
#include "raylib.h"

void createWindow() {

  // Create a temporary window to retrieve monitor info, then close it
  InitWindow(0, 0, "VoltQuest");

#if defined(__ANDROID__)

  globalSettings.monitorWidth = GetScreenWidth();
  globalSettings.monitorHeight = GetScreenHeight();

#else

  globalSettings.currentMonitor = GetCurrentMonitor();
  globalSettings.monitorWidth = GetMonitorWidth(globalSettings.currentMonitor);
  globalSettings.monitorHeight =
      GetMonitorHeight(globalSettings.currentMonitor);
#endif

  if (overrideSettings()) {
    globalSettings.screenWidth = globalSettings.monitorWidth;
    globalSettings.screenHeight = globalSettings.monitorHeight;
    globalSettings.refreshRate = 60;
    globalSettings.fullscreen = true;
    saveSettings();
  }

  if (globalSettings.fullscreen) {
    globalSettings.screenWidth = globalSettings.monitorWidth;
    globalSettings.screenHeight = globalSettings.monitorHeight;

    SetConfigFlags(FLAG_FULLSCREEN_MODE);
  } else {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  }

  // Create a the game window
  SetWindowSize(globalSettings.screenWidth, globalSettings.screenHeight);

  SetExitKey(0); // Disables Escape key from CloseWindow
  SetTargetFPS(globalSettings.refreshRate);
}
