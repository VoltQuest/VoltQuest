#include "../include/level_manager.hpp"
#include "../include/game_objects/electronic_components/electronics_base.hpp"
#include "../include/game_objects/electronic_components/passive_components.hpp"
#include "../include/game_objects/electronic_components/wire.hpp"
#include "../include/game_objects/movable_object.hpp"
#include "../include/path_utils.hpp"
#include "../include/texture_manager.hpp"
#include "../include/ui_manager.hpp"
#include "raylib.h"
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

// ───── InputManager Namespace ─────
namespace InputManager {
Vector2 mouse_pos;
MovableObject *activeDrag = nullptr;

void updateMousePos();
void updateDragInputs(MovableObject &gameObject);
} // namespace InputManager

// ───── ElectronicsLevel Namespace ─────
namespace ElectronicsLevel {
std::vector<std::shared_ptr<ElectronicComponent>> objects;
std::shared_ptr<ElectronicComponent> activeObject = nullptr;
std::vector<Wire> wires;
bool is_placing_wire = false;
std::shared_ptr<ElectronicComponent> wireStartObject = nullptr;
Pin *wireStartPin = nullptr;

void processLevel();
void resetLevel();
void loadTextures();
void updateLevel();
void drawLevel();
void drawComponentsPanel(
    std::vector<std::shared_ptr<ElectronicComponent>> &objects,
    std::shared_ptr<ElectronicComponent> &activeObject,
    std::vector<Wire> &wires, bool &isPlacingWire,
    std::shared_ptr<ElectronicComponent> &wireStartObject);
} // namespace ElectronicsLevel

// ───── InputManager Implementation ─────
void InputManager::updateMousePos() { mouse_pos = GetMousePosition(); }

void InputManager::updateDragInputs(MovableObject &gameObject) {
  Vector2 inputPos = {0, 0};
  bool inputDown = false;
  bool inputPressed = false;
  bool inputReleased = false;

  // Static variables for tracking drag state per object
  static std::unordered_map<MovableObject *, Vector2> dragOffsets;
  static std::unordered_map<MovableObject *, int> dragTouchIds;

  int &drag_touch_id = dragTouchIds[&gameObject];
  if (dragTouchIds.find(&gameObject) == dragTouchIds.end()) {
    drag_touch_id = -1;
  }

  // --- Mouse Handling ---
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    inputPos = GetMousePosition();
    inputPressed = true;
    inputDown = true;
  } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    inputPos = GetMousePosition();
    inputDown = true;
  } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    inputReleased = true;
  }

  // --- Touch Handling ---
  int touchCount = GetTouchPointCount();
  if (touchCount > 0) {
    if (drag_touch_id == -1) {
      for (int i = 0; i < touchCount; i++) {
        Vector2 tp = GetTouchPosition(i);
        Rectangle collision = gameObject.getCollider();
        if (CheckCollisionPointRec(tp, collision)) {
          inputPos = tp;
          inputPressed = true;
          inputDown = true;
          drag_touch_id = GetTouchPointId(i);
          break;
        }
      }
    } else {
      bool found = false;
      for (int i = 0; i < touchCount; i++) {
        if (GetTouchPointId(i) == drag_touch_id) {
          inputPos = GetTouchPosition(i);
          inputDown = true;
          found = true;
          break;
        }
      }
      if (!found) {
        inputReleased = true;
        drag_touch_id = -1;
      }
    }
  }

  Rectangle collision = gameObject.getCollider();

  // --- Start Dragging ---
  if (inputPressed && CheckCollisionPointRec(inputPos, collision)) {
    if (activeDrag == nullptr) {
      dragOffsets[&gameObject] = {inputPos.x - gameObject.position.x,
                                  inputPos.y - gameObject.position.y};
      gameObject.is_dragged = true;
      gameObject.is_active = true; // Set active when starting drag
      activeDrag = &gameObject;
    }
  }

  // --- Stop Dragging ---
  if (inputReleased && gameObject.is_dragged) {
    gameObject.is_dragged = false; // Stop dragging
    // BUT keep is_active = true (stays selected)
    if (activeDrag == &gameObject) {
      activeDrag = nullptr;
    }
  }

  // --- Move Only If Still Dragging ---
  if (gameObject.is_dragged && inputDown && activeDrag == &gameObject) {
    gameObject.position = {inputPos.x - dragOffsets[&gameObject].x,
                           inputPos.y - dragOffsets[&gameObject].y};
  }
}

// ───── ElectronicsLevel Implementation ─────
void ElectronicsLevel::processLevel() {
  InputManager::updateMousePos();
  updateLevel();
  drawLevel();
}

void ElectronicsLevel::resetLevel() {
  objects.clear();
  wires.clear();
  activeObject = nullptr;
  is_placing_wire = false;
  wireStartObject = nullptr;
  wireStartPin = nullptr;
  InputManager::activeDrag = nullptr;
}

void ElectronicsLevel::loadTextures() {
  TextureManager::LoadSVG(
      "battery", getResourcePath("assets/images/battery.svg"), safeScreenScale);
  TextureManager::LoadSVG("led", getResourcePath("assets/images/led.svg"),
                          safeScreenScale);
  TextureManager::LoadSVG("resistor",
                          getResourcePath("assets/images/resistor.svg"),
                          safeScreenScale);
}

void ElectronicsLevel::updateLevel() {
  // Handle left-click input for pin connections AND object selection
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    bool clickedOnPin = false;

    // Try clicking on a Pin first
    for (auto &obj : objects) {
      for (auto &pin : obj->pins) {
        if (CheckCollisionPointRec(InputManager::mouse_pos, pin.collider)) {
          clickedOnPin = true;
          if (!is_placing_wire) {
            // Start placing wire
            wireStartObject = obj;
            wireStartPin = &pin;
            is_placing_wire = true;
          } else if (wireStartObject && wireStartPin && &pin != wireStartPin) {
            // Complete wire connection
            wires.emplace_back(wireStartPin, &pin);
            is_placing_wire = false;
            wireStartObject = nullptr;
            wireStartPin = nullptr;
          }
          return;
        }
      }
    }

    // If didn't click on pin, check if clicked on an object to select it
    if (!clickedOnPin) {
      bool clickedOnObject = false;

      for (auto &obj : objects) {
        Rectangle collider = obj->getCollider();
        if (CheckCollisionPointRec(InputManager::mouse_pos, collider)) {
          // Deselect all other objects
          for (auto &other : objects) {
            if (other != obj) {
              other->is_active = false;
            }
          }
          // Select this object
          obj->is_active = true;
          activeObject = obj;
          clickedOnObject = true;
          break;
        }
      }

      // If clicked on empty space, deselect all
      if (!clickedOnObject) {
        for (auto &obj : objects) {
          obj->is_active = false;
        }
        activeObject = nullptr;
      }
    }
  }

  // Update all components - including drag inputs
  for (int i = 0; i < objects.size(); ++i) {
    // Update drag input for this object
    InputManager::updateDragInputs(*objects[i]);

    // Regular component update
    objects[i]->update();

    // Delete component if active and DELETE pressed
    if (objects[i]->is_active && IsKeyPressed(KEY_DELETE)) {
      // remove all wires connected to any pin of this object
      auto &pins = objects[i]->pins;
      wires.erase(std::remove_if(wires.begin(), wires.end(),
                                 [&](const Wire &w) {
                                   for (auto &pin : pins) {
                                     if (w.start_pin == &pin ||
                                         w.end_pin == &pin)
                                       return true;
                                   }
                                   return false;
                                 }),
                  wires.end());
      // now remove the object itself
      objects[i]->is_active = false;
      objects[i]->is_dragged = false;
      activeObject = nullptr;
      InputManager::activeDrag = nullptr;
      objects.erase(objects.begin() + i);
      break;
    }
  }
}

void ElectronicsLevel::drawLevel() {
  BeginDrawing();
  ClearBackground(GRAY);

  // Draw all components
  for (auto &obj : objects) {
    obj->draw();
  }

  // Draw all wires
  for (const Wire &wire : wires) {
    wire.draw();
  }

  // Draw preview wire
  if (is_placing_wire && wireStartObject && wireStartPin) {
    Vector2 start = wireStartPin->getCenterPosition();
    Color wireColor = wireStartPin->color;
    DrawLineEx(start, InputManager::mouse_pos, 8.0f * safeScreenScale,
               BLACK); // Outline
    DrawLineEx(start, InputManager::mouse_pos, 6.0f * safeScreenScale,
               wireColor); // Actual wire
  }
  // Draw UI side panel
  drawComponentsPanel(objects, activeObject, wires, is_placing_wire,
                      wireStartObject);
  EndDrawing();
}

void ElectronicsLevel::drawComponentsPanel(
    std::vector<std::shared_ptr<ElectronicComponent>> &objects,
    std::shared_ptr<ElectronicComponent> &activeObject,
    std::vector<Wire> &wires, bool &isPlacingWire,
    std::shared_ptr<ElectronicComponent> &wireStartObject) {
  float panelWidth = 450.0f * safeScreenScale;
  Rectangle panelBounds = {
      globalSettings.screenWidth - panelWidth,
      0.0f,
      panelWidth,
      static_cast<float>(globalSettings.screenHeight),
  };

  drawUIPanel(panelBounds);

  float margin = 22 * safeScreenScale;
  float btnSize = 100 * safeScreenScale;
  float spacing = 20 * safeScreenScale;
  int columns = 2;

  std::vector<std::string> componentNames = {"Battery", "Led", "Resistor"};
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

    drawUIRect(8.0f * safeScreenScale, 0.2f, btnRect);
    drawUITextCentered(static_cast<int>(18 * safeScreenScale), btnRect,
                       componentNames[i], DARKGRAY);

    if (CheckCollisionPointRec(GetMousePosition(), btnRect) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      const std::string &name = componentNames[i];

      if (name == "Battery") {
        auto newObj = std::make_shared<Battery>(Vector2{100, 100});
        objects.push_back(newObj);
      } else if (name == "Led") {
        auto newObj = std::make_shared<Led>(Vector2{100, 100});
        objects.push_back(newObj);
      } else if (name == "Resistor") {
        auto newObj = std::make_shared<Resistor>(Vector2{100, 100});
        objects.push_back(newObj);
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
  float textX = startX - 20.0f;

  std::vector<std::string> lines;
  lines.push_back("Inspector");
  if (activeObject) {
    lines.push_back("Position: (" +
                    std::to_string((int)activeObject->position.x) + ", " +
                    std::to_string((int)activeObject->position.y) + ")");

    if (activeObject->label == ComponentLabel::Battery) {
      lines.push_back("TYPE: Battery");
      lines.push_back("Volt: 1.5V");
    } else if (activeObject->label == ComponentLabel::Led) {
      lines.push_back("Type: LED");
      lines.push_back(std::string("State: ") +
                      (activeObject->powered
                           ? "ON"
                           : (activeObject->damaged ? "DAMAGED" : "OFF")));
    } else {
      lines.push_back("Type: Unknown");
    }
  }
  for (size_t i = 0; i < lines.size(); ++i) {
    DrawText(lines[i].c_str(), textX, inspectorStartY + i * lineSpacing,
             (i == 0 ? labelFontSize : valueFontSize), DARKGRAY);
  }
  Rectangle resetBtn = {panelBounds.x + panelBounds.width / 2.0f,
                        panelBounds.y + panelBounds.height -
                            160.0f * safeScreenScale,
                        160 * safeScreenScale, 40 * safeScreenScale};
  DrawRectangleRec(resetBtn, RED);
  DrawText("Reset Level", resetBtn.x + 20, resetBtn.y + 10, 20, WHITE);

  if (CheckCollisionPointRec(GetMousePosition(), resetBtn) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    ElectronicsLevel::resetLevel();
  }
}
