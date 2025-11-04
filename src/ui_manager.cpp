#include "../include/ui_manager.hpp"
#include "../include/level_manager.hpp"
#include "../include/path_utils.hpp"
#include "../include/texture_manager.hpp"
#include "../include/ui_utils.hpp"
#include "raylib.h"
#include <cmath>
#include <memory>
#include <string>
#include <vector>

void resetLevel();

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

void drawImage(const std::string &name, const Rectangle &bounds) {
  const Texture2D &texture = TextureManager::Get(name);
  DrawTexturePro(texture,
                 {0.0f, 0.0f, (float)texture.width, (float)texture.height},
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
