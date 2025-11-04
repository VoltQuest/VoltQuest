#ifndef ACTIVE_COMPONENTS_HPP
#define ACTIVE_COMPONENTS_HPP

#include "../../../include/path_utils.hpp"
#include "../../texture_manager.hpp"
#include "../../ui_utils.hpp"
#include "electronics_base.hpp"
#include "raylib.h"
#include <memory>
#include <string>

struct Led : public ElectronicComponent {
  ComponentLabel label = ComponentLabel::Led;

  const Texture2D &component_texture = TextureManager::Get("led");

  static constexpr float BASE_WIDTH = 60.0f;
  static constexpr float BASE_HEIGHT = 180.0f;
  static constexpr float BASE_COLLIDER_OFFSET_Y = 24.0f;
  static constexpr float BASE_PIN1_X = 21.0f;
  static constexpr float BASE_PIN1_Y = 178.0f;
  static constexpr float BASE_PIN2_X = 42.0f;
  static constexpr float BASE_PIN2_Y = 154.0f;
  static constexpr float BASE_SPRITE_OFFSET_X = 80.0f;   // for powered state
  static constexpr float BASE_DAMAGED_OFFSET_X = 160.0f; // for damaged state
  static constexpr float BASE_SELECTION_OFFSET = 5.0f;
  static constexpr float BASE_SELECTION_POWERED_OFFSET_Y = 13.0f;
  static constexpr float BASE_SELECTION_THICKNESS = 3.0f;
  static constexpr float BASE_SELECTION_PADDING = 10.0f;

  Rectangle collider = {
      position.x, position.y + BASE_COLLIDER_OFFSET_Y *screenScaleY,
      BASE_WIDTH *safeScreenScale, BASE_HEIGHT *safeScreenScale};

  Rectangle texture_box = {0.0f, 0.0f, BASE_WIDTH *safeScreenScale,
                           BASE_HEIGHT *safeScreenScale};

  Led(Vector2 pos = {0, 0}) : ElectronicComponent(ComponentLabel::Led, pos) {
    voltage = 1.9f; // sets base class voltage, used by simulation
    current = 0.02f;
    pins.emplace_back(
        Vector2{BASE_PIN1_X * screenScaleX, BASE_PIN1_Y * screenScaleY},
        PinType::Power);
    pins.emplace_back(
        Vector2{BASE_PIN2_X * screenScaleX, BASE_PIN2_Y * screenScaleY},
        PinType::Ground);
  }

  Rectangle getCollider() const override { return collider; }

  void update() override {
    collider.x = position.x;
    collider.y = position.y + BASE_COLLIDER_OFFSET_Y * screenScaleY;

    if (powered) {
      texture_box.x = BASE_SPRITE_OFFSET_X * safeScreenScale;
    } else if (damaged) {
      texture_box.x = BASE_DAMAGED_OFFSET_X * safeScreenScale;
    } else {
      texture_box.x = 0.0f;
    }

    for (auto &pin : pins) {
      pin.updateCollider(position);
    }
  }

  void draw() override {
    DrawTexturePro(
        TextureManager::Get("led"), texture_box,
        {position.x, position.y, texture_box.width, texture_box.height},
        {0.0f, 0.0f}, 0.0f, WHITE);
    drawPins();

    if (is_active) {
      DrawRectangleLinesEx(
          Rectangle{
              position.x - BASE_SELECTION_OFFSET * safeScreenScale,
              (!powered && !damaged)
                  ? position.y +
                        BASE_SELECTION_POWERED_OFFSET_Y * safeScreenScale
                  : position.y,
              texture_box.width + BASE_SELECTION_PADDING * safeScreenScale,
              texture_box.height + BASE_SELECTION_PADDING * safeScreenScale},
          BASE_SELECTION_THICKNESS * safeScreenScale, WHITE);
    }
  }
};
#endif
