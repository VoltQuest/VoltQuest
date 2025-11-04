#include "../include/level_manager.hpp"
#include "../include/path_utils.hpp"
#include "../include/screen_manager.hpp"
#include "../include/settings.hpp"
#include "../include/ui_manager.hpp"
#include "../include/ui_utils.hpp"
#include "../include/window_manager.hpp"
#include "raylib.h"
#include <cstdio>

#if defined(EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif

void gameLoop() {
  if (!globalSettings.isGameRunning) {
#if defined(EMSCRIPTEN)
    emscripten_cancel_main_loop(); // stop main loop when exiting
#endif
    return;
  }

  drawCurrentScreen();
}

int main() {
  initBasePath();
  initSettingsPath();
  loadSettings();
  createWindow();
  calculateScreenScale();
  updateLayout();

  ElectronicsLevel::loadTextures();
  TextureManager::LoadSVG("voltquest_logo",
                          getResourcePath("assets/logos/voltquest.svg"),
                          safeScreenScale);
#if defined(EMSCRIPTEN)
  emscripten_set_main_loop(gameLoop, 0, 1);
#else
  while (globalSettings.isGameRunning) {
    gameLoop();
  }
#endif

  CloseWindow();
  return 0;
}
