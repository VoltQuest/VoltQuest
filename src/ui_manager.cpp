#include "../include/ui_manager.hpp"
#include "../include/game_objects/electronic_components/output/led.hpp"
#include "../include/game_objects/electronic_components/power/battery.hpp"
#include "../include/game_objects/electronic_components/wiring/wire.hpp"
#include "../include/path_utils.hpp"
#include "../include/ui_utils.hpp"
#include "raylib.h"
#include <cmath>
#include <memory>
#include <string>
#include <vector>

static Texture2D textures[IMGCOUNT];
void resetLevel();

// ─────────────────────────────────────────────────────
// Manage Textures
// ─────────────────────────────────────────────────────
void loadAllUITextures() {
  textures[IMGLOGO] =
      LoadTexture(getResourcePath("assets/logos/VoltQuest.png").c_str());
}

void loadUITexture(int IMG_ID, const std::string &texturePath) {
  textures[IMG_ID] = LoadTexture(getResourcePath(texturePath).c_str());
}

void unloadAllUITexture() {
  for (int i = 0; i < IMGCOUNT; ++i) {
    UnloadTexture(textures[i]);
  }
}

void unloadUITexture(int IMG_ID) { UnloadTexture(textures[IMG_ID]); }

// ─────────────────────────────────────────────────────
// Draw Functions
// ─────────────────────────────────────────────────────
void drawUIRect(float outlineSize, float roundness, const Rectangle &bounds) {
  DrawRectangleRounded(bounds, roundness, 6, Color{151, 151, 165, 255});
  DrawRectangleRounded(
      Rectangle{bounds.x + (outlineSize * safeScreenScale) / 2.0f,
                bounds.y + (outlineSize * safeScreenScale) / 2.0f,
                bounds.width - outlineSize * safeScreenScale,
                bounds.height - outlineSize * safeScreenScale},
      roundness, 6, WHITE);
  DrawRectangleRounded(
      Rectangle{bounds.x + ((outlineSize * 2.0f) * safeScreenScale) / 2.0f,
                bounds.y + ((outlineSize * 2.0f) * safeScreenScale) / 2.0f,
                bounds.width - (outlineSize * 2.0f) * safeScreenScale,
                bounds.height - (outlineSize * 2.0f) * safeScreenScale},
      roundness, 6, Color{219, 221, 232, 255});
}

void drawUIButton(const UIButton &button) {
  if (button.isfocused) {
    DrawRectangleRoundedLinesEx(button.bounds, 0.15f, 6, 5.0f * safeScreenScale,
                                SKYBLUE);
  }

  drawUIRect(12.0f, 0.15f, button.bounds);
  drawUITextCentered(button.fontSize, button.bounds, button.text,
                     button.textColor);
}

void drawUIPanel(const Rectangle &bounds) {
  float outlineSize = 18.0f;
  float roundness = 0.1f;
  drawUIRect(outlineSize, roundness, bounds);
}

void drawImage(int IMG_ID, const Rectangle &bounds) {
  DrawTexturePro(textures[IMG_ID],
                 {0.0f, 0.0f, (float)textures[IMG_ID].width,
                  (float)textures[IMG_ID].height},
                 bounds, {0.0f, 0.0f}, 0.0f, WHITE);
}

void drawUIText(int fontSize, const Vector2 &textPos, const std::string &text,
                const Color &textColor) {
  int scaledFontSize = fontSize * safeScreenScale;
  int textWidth = MeasureText(text.c_str(), scaledFontSize);
  DrawText(text.c_str(), textPos.x - textWidth / 2.0f, textPos.y,
           scaledFontSize, textColor);
}

void drawUITextCentered(int fontSize, const Rectangle &bounds,
                        const std::string &text, const Color &textColor) {
  int scaledFontSize = fontSize * safeScreenScale;
  int textWidth = MeasureText(text.c_str(), scaledFontSize);

  Vector2 textPos = {
      bounds.x + (bounds.width - textWidth) / 2.0f,
      bounds.y + (bounds.height - scaledFontSize) / 2.0f,
  };
  DrawText(text.c_str(), textPos.x, textPos.y, scaledFontSize, textColor);
}

// ─────────────────────────────────────────────────────
// InGame UI functions
// ─────────────────────────────────────────────────────

void drawComponentsPanel(std::vector<std::shared_ptr<MovableObject>> &objects,
                         std::shared_ptr<MovableObject> &activeObject,
                         std::vector<Wire> &wires, bool &isPlacingWire,
                         std::shared_ptr<MovableObject> wireStartObject) {
  float panelWidth = 450.0f * safeScreenScale;
  Rectangle panelBounds = {
      globalSettings.screenWidth - panelWidth,
      0,
      panelWidth,
      static_cast<float>(globalSettings.screenHeight),
  };

  drawUIPanel(panelBounds);

  float margin = 32 * safeScreenScale;
  float btnSize = 100 * safeScreenScale;
  float spacing = 20 * safeScreenScale;
  int columns = 2;

  std::vector<std::string> componentNames = {"Battery", "LED"};
  int totalButtons = componentNames.size();

  float totalGridWidth = columns * btnSize + (columns - 1) * spacing;
  float startX = panelBounds.x + (panelBounds.width - totalGridWidth) / 2.0f;
  float startY = panelBounds.y + margin;

  // ───── Component Button Grid ─────
  for (int i = 0; i < totalButtons; ++i) {
    int col = i % columns;
    int row = i / columns;

    Rectangle btnRect = {
        startX + col * (btnSize + spacing),
        startY + row * (btnSize + spacing),
        btnSize,
        btnSize,
    };

    drawUIRect(8.0f, 0.2f, btnRect);
    drawUITextCentered(18, btnRect, componentNames[i], DARKGRAY);

    if (CheckCollisionPointRec(GetMousePosition(), btnRect) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      const std::string &name = componentNames[i];

      if (name == "Battery") {
        auto newObj = std::make_shared<Battery>(Vector2{100, 100});
        newObj->loadObjectTexture();
        objects.push_back(newObj);
      } else if (name == "LED") {
        auto newObj = std::make_shared<Led>(Vector2{100, 100});
        newObj->loadObjectTexture();
        objects.push_back(newObj);
      } else if (name == "Wire") {
        isPlacingWire = true;
        wireStartObject = nullptr;
      }
    }
  }

  // ───── ESC to Cancel Wire Mode ─────
  if (isPlacingWire && IsKeyPressed(KEY_ESCAPE)) {
    isPlacingWire = false;
    wireStartObject = nullptr;
  }

  // ───── Show ESC Hint ─────
  if (isPlacingWire) {
    DrawText("Press ESC to cancel wire", panelBounds.x + margin,
             globalSettings.screenHeight - margin, 20, DARKGRAY);
  }

  // ───── Divider Line ─────
  float dividerY = panelBounds.y + panelBounds.height / 2.0f;
  DrawLineEx(
      Vector2{panelBounds.x + margin - 5.0f, dividerY},
      Vector2{panelBounds.x + panelBounds.width - margin + 5.0f, dividerY},
      5.0f, Color{180, 180, 200, 255});

  // ───── Inspector Section ─────
  float inspectorStartY = dividerY + margin;
  float labelFontSize = 28.0f * safeScreenScale;
  float valueFontSize = 24.0f * safeScreenScale;
  float lineSpacing = 36.0f * safeScreenScale;
  float textX = startX;

  if (activeObject) {
    std::vector<std::string> lines;
    lines.push_back("Inspector");
    lines.push_back("Position: (" +
                    std::to_string((int)activeObject->position.x) + ", " +
                    std::to_string((int)activeObject->position.y) + ")");

    if (auto battery = std::dynamic_pointer_cast<Battery>(activeObject)) {
      lines.push_back("Type: Battery");
      lines.push_back("Volt: 1.5V");
    } else if (auto led = std::dynamic_pointer_cast<Led>(activeObject)) {
      lines.push_back("Type: LED");
      lines.push_back(std::string("State: ") + (led->status ? "ON" : "OFF"));
    } else {
      lines.push_back("Type: Unknown");
    }

    for (size_t i = 0; i < lines.size(); ++i) {
      DrawText(lines[i].c_str(), textX, inspectorStartY + i * lineSpacing,
               (i == 0 ? labelFontSize : valueFontSize), DARKGRAY);
    }
  }
  Rectangle resetBtn = {globalSettings.screenWidth - 300.0f,
                        globalSettings.screenHeight - 100.0f, 160, 40};
  DrawRectangleRec(resetBtn, RED);
  DrawText("Reset Level", resetBtn.x + 20, resetBtn.y + 10, 20, WHITE);

  if (CheckCollisionPointRec(GetMousePosition(), resetBtn) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    resetLevel();
  }
}

// ─────────────────────────────────────────────────────
// Input Functions
// ─────────────────────────────────────────────────────
bool isUIButtonPressed(const UIButton &button) {
  Vector2 inputPos = GetMousePosition();
  bool isHovered = CheckCollisionPointRec(inputPos, button.bounds);

  if (IsGestureDetected(GESTURE_TAP)) {
    inputPos = GetTouchPosition(0);
    isHovered = CheckCollisionPointRec(inputPos, button.bounds);
  }

  return isHovered && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                       IsGestureDetected(GESTURE_TAP)) ||
         (button.isfocused &&
          (IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_KP_ENTER)));
}

void updateKeyboardNavigation(int count, int &focusedButton,
                              UIButton **buttons) {
  if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && focusedButton > 0) {
    --focusedButton;
  } else if ((IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) &&
             focusedButton < count - 1) {
    ++focusedButton;
  }

  for (int i = 0; i < count; ++i) {
    buttons[i]->isfocused = (i == focusedButton);
  }
}
