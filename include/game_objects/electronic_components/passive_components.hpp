#ifndef PASSIVE_COMPONENTS_HPP
#define PASSIVE_COMPONENTS_HPP
#include "../../../include/input_manager.hpp"
#include "../../../include/path_utils.hpp"
#include "../../../include/ui_utils.hpp"
#include "../../texture_manager.hpp"
#include "electronics_base.hpp"
#include <raylib.h>
#include <string>

struct Resistor : public ElectronicComponent {

  const Texture2D &component_texture = TextureManager::Get("resistor");
  // Base dimensions and offsets as constants
  static constexpr float BASE_WIDTH = 215.0f;
  static constexpr float BASE_HEIGHT = 45.0f;
  static constexpr float BASE_PIN1_X = 2.0f;   // Power pin
  static constexpr float BASE_PIN2_X = 213.0f; // Ground pin
  static constexpr float BASE_PIN_Y = 20.0f;
  static constexpr float BASE_SELECTION_OFFSET = 5.0f;
  static constexpr float BASE_SELECTION_THICKNESS = 3.0f;
  static constexpr float BASE_SELECTION_PADDING = 10.0f;

  Rectangle BAND1 = {position.x + 80.0f * screenScaleX,
                     position.y + 8.0f * screenScaleY, 6.0f * safeScreenScale,
                     28.0f * safeScreenScale};
  Rectangle BAND2 = {position.x + 89.0f * screenScaleX,
                     position.y + 8.0f * screenScaleY, 6.0f * safeScreenScale,
                     28.0f * safeScreenScale};
  Rectangle BAND3 = {position.x + 98.0f * screenScaleX,
                     position.y + 8.0f * screenScaleY, 6.0f * safeScreenScale,
                     28.0f * safeScreenScale};
  Rectangle BAND4 = {position.x + 129.0f * screenScaleX,
                     position.y + 8.0f * screenScaleY, 6.0f * safeScreenScale,
                     28.0f * safeScreenScale};

  static constexpr float DEFAULT_RESISTANCE = 0.0470f; //

  Rectangle collider = {position.x, position.y, BASE_WIDTH *safeScreenScale,
                        BASE_HEIGHT *safeScreenScale};

  Resistor(Vector2 pos = {0, 0})
      : ElectronicComponent(ComponentLabel::Resistor, pos) {
    resistance = DEFAULT_RESISTANCE;
    pins.emplace_back(
        Vector2{BASE_PIN1_X * screenScaleX, BASE_PIN_Y * screenScaleY},
        PinType::BiDirectional);
    pins.emplace_back(
        Vector2{BASE_PIN2_X * screenScaleX, BASE_PIN_Y * screenScaleY},
        PinType::BiDirectional);
  }

  Rectangle getCollider() const override { return collider; }

  void update() override {
    BAND1 = {position.x + 80.0f * screenScaleX,
             position.y + 8.0f * screenScaleY, 6.0f * safeScreenScale,
             28.0f * safeScreenScale};
    BAND2 = {position.x + 89.0f * screenScaleX,
             position.y + 8.0f * screenScaleY, 6.0f * safeScreenScale,
             28.0f * safeScreenScale};
    BAND3 = {position.x + 98.0f * screenScaleX,
             position.y + 8.0f * screenScaleY, 6.0f * safeScreenScale,
             28.0f * safeScreenScale};
    BAND4 = {position.x + 129.0f * screenScaleX,
             position.y + 8.0f * screenScaleY, 6.0f * safeScreenScale,
             28.0f * safeScreenScale};
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
    DrawRectangleRec(BAND1, RED);
    DrawRectangleRec(BAND2, GRAY);
    DrawRectangleRec(BAND3, BROWN);
    DrawRectangleRec(BAND4, GOLD);
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
