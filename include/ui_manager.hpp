#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "../include/game_objects/electronic_components/wiring/wire.hpp"
#include "../include/game_objects/movable_object.hpp"
#include "raylib.h"
#include <memory>
#include <string>
#include <vector>

constexpr int IMGLOGO = 0;
constexpr int IMGCOUNT = 1;

struct UIButton {
  Rectangle bounds;
  std::string text;
  int fontSize;
  Color textColor;
  bool isfocused;
};

// Manage Textures
void loadAllUITextures();
void loadUITexture(int IMG_ID, const std::string &texturePath);
void unloadAllUITexture();
void unloadUITexture(int IMG_ID);

// Draw Functions
void drawUIRect(float outlineSize, const Rectangle &bounds);
void drawUIButton(const UIButton &button);
void drawImage(int IMG_ID, const Rectangle &bounds);
void drawUIPanel(float outlineSize, const Rectangle &bounds);
void drawUIText(int fontSize, const Vector2 &textPos, const std::string &text,
                const Color &textColor);
void drawUITextCentered(int fontSize, const Rectangle &bounds,
                        const std::string &text, const Color &textColor);

// Ingame UI Draw Functions
void drawComponentsPanel(
    std::vector<std::shared_ptr<MovableObject>> &objects,
    std::shared_ptr<MovableObject> &activeObject, std::vector<Wire> &wires,
    bool &isPlacingWire,
    std::shared_ptr<MovableObject> wireStartObject = nullptr);

// Input Functions
bool isUIButtonPressed(const UIButton &button);
void updateKeyboardNavigation(int count, int &focusedButton,
                              UIButton **buttons);

#endif
