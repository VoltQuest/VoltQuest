#include "../include/level_manager.hpp"
#include "../include/game_objects/electronic_components/active_components.hpp"
#include "../include/game_objects/electronic_components/passive_components.hpp"
#include "../include/game_objects/electronic_components/power_sources.hpp"
#include "../include/input_manager.hpp"
#include "../include/texture_manager.hpp"
#include "../include/ui_utils.hpp"

#include <algorithm>
#include <iostream>
#include <string>

static constexpr float SNAP_RADIUS_PX = 10.0f;

ElectronicsLevel::ElectronicsLevel() {}
ElectronicsLevel::~ElectronicsLevel() {}

void ElectronicsLevel::processLevel() {
  InputManager::updateMousePos();
  updateLevel();
  drawLevel();
  for (auto c : connections) {
    int i = 0;
    std::cout << "wire:" << i << '\n';
    i++;
  }
}

void ElectronicsLevel::resetLevel() {
  objects.clear();
  connections.clear();
  activeObject = nullptr;
  is_placing_wire = false;
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

// Helpers
Pin *ElectronicsLevel::findSnapTarget(Pin *source, float radius) const {
  Vector2 a = source->getCenterPosition();

  for (auto &obj : objects) {
    for (auto &pin : obj->pins) {
      Pin *p = &pin;
      if (p == source)
        continue;

      Vector2 b = p->getCenterPosition();
      float dx = a.x - b.x;
      float dy = a.y - b.y;

      if ((dx * dx + dy * dy) <= radius * radius)
        return p;
    }
  }
  return nullptr;
}

bool ElectronicsLevel::hasConnection(Pin *a, Pin *b) const {
  for (const auto &c : connections) {
    if ((c.getPin(0) == a && c.getPin(1) == b) ||
        (c.getPin(0) == b && c.getPin(1) == a))
      return true;
  }
  return false;
}

// Update
void ElectronicsLevel::updateLevel() {
  Vector2 mouse = InputManager::GetCachedMousePos();
  bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

  // click handling
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    for (auto &obj : objects) {
      for (auto &pin : obj->pins) {
        if (pin.isHovered()) {

          if (!is_placing_wire) {
            wireStartPin = &pin;
            is_placing_wire = true;
          } else if (wireStartPin && &pin != wireStartPin) {
            if (!hasConnection(wireStartPin, &pin))
              connections.emplace_back(wireStartPin, &pin);

            wireStartPin = nullptr;
            is_placing_wire = false;
          }
          return;
        }
      }
    }

    // object selection
    bool hit = false;
    for (auto &obj : objects) {
      if (CheckCollisionPointRec(mouse, obj->getCollider())) {
        for (auto &o : objects)
          o->is_active = false;
        obj->is_active = true;
        activeObject = obj;
        hit = true;
        break;
      }
    }

    if (!hit) {
      for (auto &o : objects)
        o->is_active = false;
      activeObject = nullptr;
    }
  }

  // update objects
  for (int i = 0; i < objects.size(); ++i) {
    InputManager::updateDragInputs(*objects[i]);
    objects[i]->update();

    if (objects[i]->is_active && IsKeyPressed(KEY_DELETE)) {

      auto &pins = objects[i]->pins;
      connections.erase(std::remove_if(connections.begin(), connections.end(),
                                       [&](const Connection &c) {
                                         for (auto &p : pins)
                                           if (c.getPin(0) == &p ||
                                               c.getPin(1) == &p)
                                             return true;
                                         return false;
                                       }),
                        connections.end());

      objects.erase(objects.begin() + i);
      activeObject = nullptr;
      break;
    }
  }

  if (mouseReleased) {
    float snapDist = SNAP_RADIUS_PX * safeScreenScale;

    for (auto &obj : objects) {
      for (auto &pin : obj->pins) {
        Pin *p = &pin;
        Pin *target = findSnapTarget(p, snapDist);
        if (!target)
          continue;

        if (!hasConnection(p, target)) {
          connections.emplace_back(p, target);
        }
      }
    }
  }
}

// Draw
void ElectronicsLevel::drawLevel() {
  BeginDrawing();
  ClearBackground(GRAY);

  for (auto &obj : objects)
    obj->draw();

  for (const Connection &c : connections)
    c.draw();

  // wire preview
  if (is_placing_wire && wireStartPin) {
    Vector2 a = wireStartPin->getCenterPosition();
    Vector2 b = InputManager::GetCachedMousePos();
    Color col = wireStartPin->getColor();

    DrawLineEx(a, b, 8.0f * safeScreenScale, BLACK);
    DrawLineEx(a, b, 6.0f * safeScreenScale, col);
  }

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

  // Draw buttons
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

  // Wire Cancellation
  if (is_placing_wire) {
    if (IsKeyPressed(KEY_ESCAPE)) {
      is_placing_wire = false;
      wireStartObject = nullptr;
    }
    DrawText("Press ESC to cancel wire", panelBounds.x + margin,
             globalSettings.screenHeight - margin, 20, DARKGRAY);
  }

  // Divider
  float dividerY = panelBounds.y + panelBounds.height / 2.0f;
  DrawLineEx(
      Vector2{panelBounds.x + margin - 5.0f, dividerY},
      Vector2{panelBounds.x + panelBounds.width - margin + 5.0f, dividerY},
      5.0f, Color{180, 180, 200, 255});

  // Inspector
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

  // Reset Button
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
