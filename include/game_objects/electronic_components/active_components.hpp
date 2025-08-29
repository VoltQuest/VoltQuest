#ifndef ACTIVE_COMPONENTS_HPP
#define ACTIVE_COMPONENTS_HPP

#include "../../../include/input_manager.hpp"
#include "../../../include/path_utils.hpp"
#include "../../ui_utils.hpp"
#include "electronics_base.hpp"
#include "raylib.h"
#include <string>

struct Led : public ElectronicComponent {
  ComponentLabel label = ComponentLabel::Led;
  VoltInput inputs;
  inline static Texture2D shared_texture = {0};

  Rectangle collider = {position.x, position.y + 24.0f, 60.0f, 156.0f};
  Rectangle texture_box = {0.0f, 0.0f, 60.0f, 180.0f};

  Led(Vector2 pos = {0, 0}) : ElectronicComponent(ComponentLabel::Led, pos) {
    voltage = 1.9f; // sets base class voltage, used by simulation
    current = 0.02f;
    pins.emplace_back(Vector2{12.0f, 167.0f}, PinType::Power);
    pins.emplace_back(Vector2{33.0f, 145.0f}, PinType::Ground);
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
    collider.y = position.y + 24.0f * screenScaleY;
    if (powered) {
      texture_box.x = 90.0f;
    } else if (damaged) {
      texture_box.x = 180.0f;
    } else {
      texture_box.x = 0.0f;
    }
    inputs.mouseDragRect(collider, position, is_active);
    for (auto &pin : pins) {
      pin.updateCollider(position);
    }
  }

  void draw() override {
    DrawTexturePro(texture, texture_box,
                   {position.x, position.y,
                    static_cast<float>(texture_box.width),
                    static_cast<float>(texture_box.height)},
                   {0.0f, 0.0f}, 0.0f, WHITE);

    drawPins();
    if (is_active) {
      DrawRectangleLinesEx(Rectangle{(position.x - 5.0f),
                                     (!powered && !damaged) ? position.y + 19.0f
                                                            : position.y + 5.0f,
                                     (texture_box.width + 10.0f),
                                     (texture_box.height + 10.0f)},
                           3.0f, WHITE);
    }
  }
  void unloadObjectTexture() override { UnloadTexture(shared_texture); }
};
#endif
