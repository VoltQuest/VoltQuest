#ifndef POWER_SOURCES_HPP
#define POWER_SOURCES_HPP
#include "../../../include/input_manager.hpp"
#include "../../../include/path_utils.hpp"
#include "../../../include/ui_utils.hpp"
#include "../../texture_manager.hpp"
#include "electronics_base.hpp"
#include <raylib.h>
#include <string>

struct Battery : public ElectronicComponent {
  VoltInput inputs;

  const Texture2D &component_texture = TextureManager::Get("battery");

  // Base dimensions and offsets as constants
  static constexpr float BASE_WIDTH = 110.0f;
  static constexpr float BASE_HEIGHT = 450.0f;
  static constexpr float BASE_PIN_X = 56.0f;
  static constexpr float BASE_PIN1_Y = 8.0f;   // Power pin (top)
  static constexpr float BASE_PIN2_Y = 442.0f; // Ground pin (bottom)
  static constexpr float BASE_SELECTION_OFFSET = 5.0f;
  static constexpr float BASE_SELECTION_THICKNESS = 3.0f;
  static constexpr float BASE_SELECTION_PADDING = 10.0f;

  // Battery specifications
  static constexpr float DEFAULT_VOLTAGE = 2.0f;
  static constexpr float DEFAULT_CURRENT = 0.02f;

  Rectangle collider = {position.x, position.y, BASE_WIDTH *safeScreenScale,
                        BASE_HEIGHT *safeScreenScale};

  Battery(Vector2 pos = {0, 0})
      : ElectronicComponent(ComponentLabel::Battery, pos) {
    voltage = DEFAULT_VOLTAGE; // sets base class voltage, used by simulation
    current = DEFAULT_CURRENT;
    pins.emplace_back(
        Vector2{BASE_PIN_X * screenScaleX, BASE_PIN1_Y * screenScaleY},
        PinType::Power);
    pins.emplace_back(
        Vector2{BASE_PIN_X * screenScaleX, BASE_PIN2_Y * screenScaleY},
        PinType::Ground);
  }

  Rectangle getCollider() const override { return collider; }

  void update() override {
    inputs.mouseDragRect(collider, position, is_dragged);
    collider.x = position.x;
    collider.y = position.y;
    for (auto &pin : pins) {
      pin.updateCollider(position);
    }
  }

  void draw() override {

    DrawTexturePro(component_texture,
                   {0.0f, 0.0f, static_cast<float>(component_texture.width),
                    static_cast<float>(component_texture.height)},
                   {position.x, position.y,
                    static_cast<float>(component_texture.width),
                    static_cast<float>(component_texture.height)},
                   {0.0f, 0.0f}, 0.0f, WHITE);

    drawPins();

    if (is_active) {
      DrawRectangleLinesEx(
          Rectangle{position.x - BASE_SELECTION_OFFSET,
                    position.y - BASE_SELECTION_OFFSET,
                    static_cast<float>(component_texture.width) +
                        BASE_SELECTION_PADDING,
                    static_cast<float>(component_texture.height) +
                        BASE_SELECTION_PADDING},
          BASE_SELECTION_THICKNESS, WHITE);
    }
  }

  bool isBatteryConnected();
};

#endif
