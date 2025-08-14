#ifndef ELECTRONICS_BASE_HPP
#define ELECTRONICS_BASE_HPP
#include "../movable_object.hpp"
#include "raylib.h"
#include <string>
#include <vector>

enum class PinType { Power, Ground, Input, Output, BiDirectional };

struct Pin {
  Vector2 relative_position;
  Rectangle collider;
  PinType type;
  Color color;
  std::string label;
  float voltage = 0.0f;
  float current = 0.0f;
  bool is_connected = false;

  Pin(Vector2 relPos, PinType pinType, const std::string &name,
      float colliderSize = 14.0f)
      : relative_position(relPos), type(pinType), label(name) {
    collider.width = colliderSize;
    collider.height = colliderSize;

    color = (type == PinType::Power)    ? RED
            : (type == PinType::Ground) ? BLACK
                                        : GREEN;
  }

  Vector2 getWorldPosition(const Vector2 &componentPos) const {
    return {componentPos.x + relative_position.x,
            componentPos.y + relative_position.y};
  }

  void updateCollider(const Vector2 &componentPos) {
    collider.x = componentPos.x + relative_position.x;
    collider.y = componentPos.y + relative_position.y;
  }

  Vector2 getCenterPosition() const {
    return {collider.x + collider.width / 2.0f,
            collider.y + collider.height / 2.0f};
  }
};

struct ElectronicComponent : public MovableObject {
  std::vector<Pin> pins;

  ElectronicComponent(Vector2 pos = {0, 0}, float rot = 0.0f)
      : MovableObject(pos, rot) {}

  virtual void drawPins() const {
    for (const auto &pin : pins) {

      DrawRectangle(pin.collider.x, pin.collider.y, pin.collider.width,
                    pin.collider.height, pin.color);
    }
  };

  virtual void simulate() {};
  virtual ~ElectronicComponent() = default;
};

#endif
