#include "../include/level_manager.hpp"
#include "../include/game_objects/electronic_components/electronics_base.hpp"
#include "../include/game_objects/electronic_components/wiring/wire.hpp"
#include "../include/ui_manager.hpp"
#include "raylib.h"
#include "raymath.h"
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
  for (auto &obj : objects) {
    obj->loadObjectTexture();
  }
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
  for (auto &obj : objects) {
    obj->update();
  }
}

void ElectronicsLevel::drawLevel() {
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
    DrawLineEx(start, mouse_pos, 6.0f, BLACK);     // Outline
    DrawLineEx(start, mouse_pos, 4.0f, wireColor); // Actual wire
  }

  // Draw UI side panel
  drawComponentsPanel(objects, activeObject, wires, is_placing_wire,
                      wireStartObject);
}
