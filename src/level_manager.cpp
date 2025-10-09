#include "../include/level_manager.hpp"
#include "../include/game_objects/electronic_components/electronics_base.hpp"
#include "../include/game_objects/electronic_components/passive_components.hpp"
#include "../include/game_objects/electronic_components/wire.hpp"
#include "../include/input_manager.hpp"
#include "../include/path_utils.hpp"
#include "../include/simulation/electronics_simulation.hpp"
#include "../include/texture_manager.hpp"
#include "../include/ui_manager.hpp"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <limits>
#include <memory>
#include <vector>

// Global variables
Vector2 mouse_pos;

namespace ElectronicsLevel {
std::vector<std::shared_ptr<ElectronicComponent>> objects;
std::shared_ptr<ElectronicComponent> activeObject = nullptr;
std::vector<Wire> wires;
bool is_placing_wire = false;
std::shared_ptr<ElectronicComponent> wireStartObject = nullptr;
Pin *wireStartPin = nullptr;
} // namespace ElectronicsLevel

void updateMousePos() { mouse_pos = GetMousePosition(); }

void ElectronicsLevel::processLevel() {
  updateMousePos();
  updateLevel();
  simulateLevel(objects, wires);
  drawLevel();
}

void ElectronicsLevel::resetLevel() {
  objects.clear();
  wires.clear();
  activeObject = nullptr;
  is_placing_wire = false;
  wireStartObject = nullptr;
  wireStartPin = nullptr;
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
  // Handle left-click input
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

    // Try clicking on a Pin first
    for (auto &obj : objects) {
      for (auto &pin : obj->pins) {
        if (CheckCollisionPointRec(mouse_pos, pin.collider)) {
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
  }

  // Update all components
  for (int i = 0; i < objects.size(); ++i) {
    objects[i]->update();
    if (objects[i]->is_active) {
      activeObject = objects[i];
    }

    // Delete component
    if (objects[i]->is_active && IsKeyPressed(KEY_DELETE)) {
      // remove all wires connected to any pin of this object
      auto &pins = activeObject->pins;

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
      activeObject = nullptr;
      activeDrag = nullptr;
      objects.erase(objects.begin() + i);
      break; // safe, since we deleted one object
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
    DrawLineEx(start, mouse_pos, 8.0f * safeScreenScale, BLACK); // Outline
    DrawLineEx(start, mouse_pos, 6.0f * safeScreenScale,
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
