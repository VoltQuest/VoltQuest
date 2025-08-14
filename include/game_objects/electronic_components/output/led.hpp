#ifndef LED_HPP
#define LED_HPP
#include "../../../../include/input_manager.hpp"
#include "../../../../include/path_utils.hpp"
#include "../../../ui_utils.hpp"
#include "../electronics_base.hpp"
#include "raylib.h"
#include <string>

struct Led : public ElectronicComponent {
  static constexpr float voltage_threshold = 2.0f;
  static constexpr float max_current_limit = 0.02f;
  bool is_enabled = false;
  VoltInput inputs;
  inline static Texture2D shared_texture = {0};

  Rectangle collider = {position.x, position.y - 30.0f, 60, 100.0f};

  Led(Vector2 pos = {0, 0}) : ElectronicComponent(pos) {
    pins.emplace_back(Vector2{6.0f, 93.0f}, PinType::Power, "anode");
    pins.emplace_back(Vector2{20.0f, 79.0f}, PinType::Ground, "Cathode");
  }

  void loadObjectTexture() override {
    if (shared_texture.id == 0) {
      shared_texture =
          LoadTexture(getResourcePath("assets/images/LED.png").c_str());
    }
    texture = shared_texture;
  }

  Rectangle getCollider() const override { return collider; }

  void update() override {
    collider.x = position.x;
    collider.y = position.y - 30.0f * screenScaleY;
    inputs.mouseDragRect(collider, position, is_active);
    for (auto &pin : pins) {
      pin.updateCollider(position);
    }
  }

  void simulate() override {}

  void draw() override {
    DrawTexturePro(texture,
                   {0.0f, 0.0f, static_cast<float>(texture.width),
                    static_cast<float>(texture.height)},
                   {position.x, position.y, static_cast<float>(texture.width),
                    static_cast<float>(texture.height)},
                   {0.0f, 0.0f}, 0.0f, WHITE);

    drawPins();

    if (is_active) {
      DrawRectangleLinesEx(Rectangle{(position.x - 5.0f), (position.y - 5.0f),
                                     (texture.width + 10.0f),
                                     (texture.height + 10.0f)},
                           3.0f, WHITE);
    }
  }

  void unloadObjectTexture() override { UnloadTexture(shared_texture); }
};
#endif
