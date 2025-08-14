#include "../include/screen_manager.hpp"
#include "../include/level_manager.hpp"
#include "../include/settings.hpp"
#include "../include/ui_manager.hpp"
#include "../include/ui_utils.hpp"
#include "raylib.h"
#include <cmath>
#include <iostream>
enum class SCREEN { START_MENU, OPTIONS_MENU, CHAPTER_MENU, LEVEL_MENU, GAME };
SCREEN currentScreen = SCREEN::START_MENU;

namespace startMenu {
float logoSize;
Rectangle logoBounds;
Vector2 buttonSize;
Rectangle playButtonBounds;
Rectangle optionsButtonBounds;
Rectangle quitButtonBounds;

UIButton playButton;
UIButton optionsButton;
UIButton quitButton;
UIButton *buttonsArray[3] = {&playButton, &optionsButton, &quitButton};

int focusedButton = 0;
} // namespace startMenu

namespace optionsMenu {
Vector2 panelSize;
Rectangle panelBounds;
Vector2 optionsTextPos;
} // namespace optionsMenu

void updateLayout() {
  screenScaleX = static_cast<float>(globalSettings.screenWidth) / baseWidth;
  screenScaleY = static_cast<float>(globalSettings.screenHeight) / baseHeight;
  safeScreenScale = (screenScaleX + screenScaleY) / 2.0f;
  // ─── Start Menu ────────────────────────────────
  {
    startMenu::logoSize = 450.0f * safeScreenScale;
    startMenu::buttonSize = {360.0f * safeScreenScale,
                             120.0f * safeScreenScale};
    startMenu::logoBounds = {
        (baseWidth / 2.0f * screenScaleX) - (startMenu::logoSize / 2.0f),
        (baseHeight / 2.0f * screenScaleY) - (startMenu::logoSize),
        startMenu::logoSize, startMenu::logoSize};

    startMenu::playButton = {
        {(baseWidth / 2.0f * screenScaleX) - (startMenu::buttonSize.x / 2.0f),
         (baseHeight / 2.0f * screenScaleY), startMenu::buttonSize.x,
         startMenu::buttonSize.y},
        "PLAY",
        55,
        Color{255, 198, 0, 255}};

    startMenu::optionsButton = {
        {(baseWidth / 2.0f * screenScaleX) - (startMenu::buttonSize.x / 2.0f),
         (baseHeight / 2.0f * screenScaleY) + (160.f * screenScaleY),
         startMenu::buttonSize.x, startMenu::buttonSize.y},
        "OPTIONS",
        55,
        Color{0, 146, 255, 255}};

    startMenu::quitButton = {
        {(baseWidth / 2.0f * screenScaleX) - (startMenu::buttonSize.x / 2.0f),
         (baseHeight / 2.0f * screenScaleY) + (320.f * screenScaleY),
         startMenu::buttonSize.x, startMenu::buttonSize.y},
        "QUIT",
        55,
        RED};
  }

  // ─── Options Menu ──────────────────────────────
  {
    optionsMenu::panelSize = {1000.0f * safeScreenScale,
                              900.0f * safeScreenScale}; // Static panel size
    optionsMenu::panelBounds = {
        (baseWidth / 2.0f * screenScaleX) - (optionsMenu::panelSize.x / 2.0f),
        (baseHeight / 2.0f * screenScaleY) - (optionsMenu::panelSize.y / 2.0f),
        optionsMenu::panelSize.x, optionsMenu::panelSize.y};

    optionsMenu::optionsTextPos = {baseWidth / 2.0f * screenScaleX,
                                   baseHeight / 8.0f}; // Top text Y position
  }
}

void drawStartMenu() {
  ClearBackground(Color{58, 71, 80, 255});
  drawImage(IMGLOGO, startMenu::logoBounds);
  drawUIButton(startMenu::playButton);
  drawUIButton(startMenu::optionsButton);
  drawUIButton(startMenu::quitButton);
}

void drawOptionsMenu() {
  ClearBackground(Color{58, 71, 80, 255});
  drawUIPanel(optionsMenu::panelBounds);
  drawUIText(80, optionsMenu::optionsTextPos, "OPTIONS",
             Color{0, 146, 255, 255});
}

void drawCurrentScreen() {
  switch (currentScreen) {
  case SCREEN::START_MENU: {
    drawStartMenu();
    updateKeyboardNavigation(3, startMenu::focusedButton,
                             startMenu::buttonsArray);
    if (isUIButtonPressed(startMenu::playButton)) {
      currentScreen = SCREEN::GAME;
    } else if (isUIButtonPressed(startMenu::optionsButton)) {
      currentScreen = SCREEN::OPTIONS_MENU;
    } else if (isUIButtonPressed(startMenu::quitButton)) {
      globalSettings.isGameRunning = false;
    }
    break;
  }

  case SCREEN::OPTIONS_MENU: {
    drawOptionsMenu();
    if (IsKeyDown(KEY_ESCAPE)) {
      currentScreen = SCREEN::START_MENU;
    }
    break;
  }

  case SCREEN::GAME: {
    ElectronicsLevel::processLevel();
    if (IsKeyPressed(KEY_BACKSPACE)) {
      currentScreen = SCREEN::START_MENU;
    }
  }
  default:
    break;
  }
}
