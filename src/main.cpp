#include "../include/path_utils.hpp"
#include "../include/screen_manager.hpp"
#include "../include/settings.hpp"
#include "../include/ui_manager.hpp"
#include "../include/ui_utils.hpp"
#include "../include/window_manager.hpp"
#include "raylib.h"
int main() {
  initBasePath();
  loadSettings();
  createWindow();
  calculateScreenScale();
  updateLayout();
  loadAllUITextures();
  while (globalSettings.isGameRunning) {
    BeginDrawing();
    drawCurrentScreen();
    EndDrawing();
  }
  unloadAllUITexture();
  CloseWindow();
  return 0;
}
