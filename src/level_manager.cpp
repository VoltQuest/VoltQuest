#include "../include/level_manager.hpp"
#include "../include/game_objects/electronic_components/active_components.hpp"
#include "../include/game_objects/electronic_components/passive_components.hpp"
#include "../include/game_objects/electronic_components/power_sources.hpp"
#include "../include/input_manager.hpp"
#include "../include/texture_manager.hpp"
#include "../include/ui_utils.hpp"
#include <algorithm>
#include <string>

ElectronicsLevel::ElectronicsLevel() {
  is_placing_wire = false;
  wireStartObject = nullptr;
  wireStartPin = nullptr;
  activeObject = nullptr;
}

ElectronicsLevel::~ElectronicsLevel() {}

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

  InputManager::ClearActiveSelection();
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
  Vector2 mousePos = InputManager::GetCachedMousePos();

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    bool clickedOnPin = false;

    for (auto &obj : objects) {
      for (auto &pin : obj->pins) {
        if (pin.isHovered()) {
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
          return; // Exit early if we hit a pin
        }
      }
    }

    // 2. Check Object Selection
    if (!clickedOnPin) {
      bool clickedOnObject = false;

      for (auto &obj : objects) {
        Rectangle collider = obj->getCollider();
        if (CheckCollisionPointRec(mousePos, collider)) {
          // Deselect others
          for (auto &other : objects) {
            if (other != obj)
              other->is_active = false;
          }
          // Select this
          obj->is_active = true;
          activeObject = obj;
          clickedOnObject = true;
          break;
        }
      }

      // Clicked Empty Space? Deselect all.
      if (!clickedOnObject) {
        for (auto &obj : objects)
          obj->is_active = false;
        activeObject = nullptr;
      }
    }
  }

  // ─── Update Loop & Deletion ───
  for (int i = 0; i < objects.size(); ++i) {

    // 1. Handle Dragging via InputManager
    InputManager::updateDragInputs(*objects[i]);

    // 2. Update Component Logic
    objects[i]->update();

    // 3. Handle Deletion (KEY_DELETE)
    if (objects[i]->is_active && IsKeyPressed(KEY_DELETE)) {

      if (InputManager::GetActiveSelection() == objects[i].get()) {
        InputManager::ClearActiveSelection();
      }

      // Remove connected wires
      auto &pins = objects[i]->pins;
      wires.erase(std::remove_if(wires.begin(), wires.end(),
                                 [&](const Wire &w) {
                                   for (auto &pin : pins) {
                                     if (w.getPin(0) == &pin ||
                                         w.getPin(1) == &pin)
                                       return true;
                                   }
                                   return false;
                                 }),
                  wires.end());

      // Clear activeObject pointer if it matches
      if (activeObject == objects[i]) {
        activeObject = nullptr;
      }

      // Finally, delete the object
      objects.erase(objects.begin() + i);
      break; // Stop loop because iterator 'i' is now invalid
    }
  }
}

void ElectronicsLevel::drawLevel() {
  BeginDrawing();
  ClearBackground(GRAY);

  // Draw components
  for (auto &obj : objects) {
    obj->draw();
  }

  // Draw wires
  for (const Wire &wire : wires) {
    wire.draw();
  }

  // Draw preview wire (while dragging)
  if (is_placing_wire && wireStartObject && wireStartPin) {
    Vector2 start = wireStartPin->getCenterPosition();
    Color wireColor = wireStartPin->getColor();
    Vector2 currentMouse = InputManager::GetCachedMousePos();

    DrawLineEx(start, currentMouse, 8.0f * safeScreenScale, BLACK); // Outline
    DrawLineEx(start, currentMouse, 6.0f * safeScreenScale, wireColor); // Inner
  }

  // Draw UI
  drawComponentsPanel();

  EndDrawing();
}

void ElectronicsLevel::drawComponentsPanel() {
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

  // ─── Draw Spawn Buttons ───
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
        objects.push_back(std::make_shared<Battery>(Vector2{100, 100}));
      } else if (name == "Led") {
        objects.push_back(std::make_shared<Led>(Vector2{100, 100}));
      } else if (name == "Resistor") {
        objects.push_back(std::make_shared<Resistor>(Vector2{100, 100}));
      }
    }
  }

  // ─── Wire Cancellation ───
  if (is_placing_wire) {
    if (IsKeyPressed(KEY_ESCAPE)) {
      is_placing_wire = false;
      wireStartObject = nullptr;
    }
    DrawText("Press ESC to cancel wire", panelBounds.x + margin,
             globalSettings.screenHeight - margin, 20, DARKGRAY);
  }

  // ─── Divider ───
  float dividerY = panelBounds.y + panelBounds.height / 2.0f;
  DrawLineEx(
      Vector2{panelBounds.x + margin - 5.0f, dividerY},
      Vector2{panelBounds.x + panelBounds.width - margin + 5.0f, dividerY},
      5.0f, Color{180, 180, 200, 255});

  // ─── Inspector ───
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

  // ─── Reset Button ───
  Rectangle resetBtn = {panelBounds.x + panelBounds.width / 2.0f,
                        panelBounds.y + panelBounds.height -
                            160.0f * safeScreenScale,
                        160 * safeScreenScale, 40 * safeScreenScale};
  DrawRectangleRec(resetBtn, RED);
  DrawText("Reset Level", resetBtn.x + 20, resetBtn.y + 10, 20, WHITE);

  if (CheckCollisionPointRec(GetMousePosition(), resetBtn) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    resetLevel();
  }
}
