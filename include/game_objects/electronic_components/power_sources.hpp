#ifndef POWER_SOURCES_HPP
#define POWER_SOURCES_HPP

#include "../../../include/input_manager.hpp"
#include "../../../include/path_utils.hpp"
#include "../../../include/ui_utils.hpp"
#include "electronics_base.hpp"
#include <raylib.h>
#include <string>

struct Battery : public ElectronicComponent {
  VoltInput inputs;
  inline static Texture2D shared_texture = {0};

  Rectangle collider = {position.x, position.y, 112.5f, 450.0f};

  Battery(Vector2 pos = {0, 0})
      : ElectronicComponent(ComponentLabel::Battery, pos) {
    voltage = 2.0f; // sets base class voltage, used by simulation
    current = 0.02f;

    pins.emplace_back(Vector2{46.0f, -8.0f}, PinType::Power);
    pins.emplace_back(Vector2{46.0f, 430.0f}, PinType::Ground);
  }

  void loadObjectTexture() override {
    if (shared_texture.id == 0) {
      shared_texture =
          LoadTexture(getResourcePath("assets/images/battery.png").c_str());
    }
    texture = shared_texture;
  }

  Rectangle getCollider() const override { return collider; }

  void update() override {
    inputs.mouseDragRect(collider, position, is_active);
    collider.x = position.x;
    collider.y = position.y;
    for (auto &pin : pins) {
      pin.updateCollider(position);
    }
  }

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

  bool isBatteryConnected();
};

#endif
