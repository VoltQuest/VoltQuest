#include "../include/level_manager.hpp"
#include "../include/path_utils.hpp"
#include "../include/screen_manager.hpp"
#include "../include/settings.hpp"
#include "../include/texture_manager.hpp"
#include "../include/ui_manager.hpp"
#include "../include/ui_utils.hpp"
#include "../include/window_manager.hpp"
#include "raylib.h"


int main() {
  initBasePath();
  initSettingsPath();
  loadSettings();
  createWindow();
  calculateScreenScale();
  updateLayout();
  TextureManager::LoadSVG("voltquest_logo",
                          getResourcePath("assets/logos/voltquest.svg"),
                          safeScreenScale);
  while (globalSettings.isGameRunning) {
    drawCurrentScreen();
  }
  CloseWindow();
  return 0;
}
