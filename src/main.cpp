#include "../include/screen_manager.hpp"
#include "../include/settings.hpp"
#include "../include/ui_manager.hpp"
#include "../include/window_manager.hpp"
#include "raylib.h"
int main() {
  loadSettings();
  createWindow();
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
