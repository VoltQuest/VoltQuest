#ifndef ELECTRONICS_BASE_HPP
#define ELECTRONICS_BASE_HPP
#include "../../ui_utils.hpp"
#include "../movable_object.hpp"
#include "raylib.h"
#include <cstdint>
#include <vector>

enum class PinType : uint8_t { Power, Ground, Input, Output, BiDirectional };

enum class ComponentLabel : uint8_t {
  Battery = 0,
  Led = 1,
  Resistor = 2,
  Switch = 3,
};

class Pin {

private:
  Vector2 relative_position;
  Rectangle collider;
  float collider_size = 16.0f * safeScreenScale;
  PinType type;
  float voltage = 0.0f;
  float current = 0.0f;
  bool is_connected = false;
  int16_t index = -1;
  int16_t node_id = -1;

public:
  Pin(Vector2 relPos, PinType pinType)
      : relative_position(relPos), type(pinType) {
    collider.width = collider_size;
    collider.height = collider_size;
  }

  Vector2 getWorldPosition(const Vector2 &componentPos) const {
    return {componentPos.x + relative_position.x,
            componentPos.y + relative_position.y};
  }
  Vector2 getCenterPosition() const {
    return {collider.x + collider.width / 2.0f,
            collider.y + collider.height / 2.0f};
  }

  PinType getPinType() const { return type; }

  Color getColor() const {
    return (type == PinType::Power)    ? RED
           : (type == PinType::Ground) ? BLACK
                                       : GREEN;
  }

  Vector2 getColliderPosition() const {
    return (Vector2){collider.x, collider.y};
  }

  float getColliderSize() const { return collider_size; }

  void updateCollider(const Vector2 &componentPos) {
    collider.x = componentPos.x + relative_position.x - (collider.width / 2.0f);
    collider.y =
        componentPos.y + relative_position.y - (collider.height / 2.0f);
  }

  bool isHovered() const {
    return (CheckCollisionPointRec(GetMousePosition(), collider));
  }
};

struct ElectronicComponent : public MovableObject {
  float voltage;
  float current;
  float resistance;
  bool powered = false;
  bool damaged = false;
  ComponentLabel label;
  std::vector<Pin> pins;

  ElectronicComponent(ComponentLabel component_label, Vector2 pos = {0, 0},
                      float rot = 0.0f, float init_voltage = 0.0f,
                      float init_current = 0.0f, bool init_powered = false)
      : MovableObject(pos, rot), voltage(init_voltage), current(init_current),
        powered(init_powered), label(component_label) {}

  virtual void drawPins() const {
    for (const auto &pin : pins) {
      if (pin.isHovered()) {
        DrawRectangle(pin.getColliderPosition().x, pin.getColliderPosition().y,
                      pin.getColliderSize(), pin.getColliderSize(),
                      pin.getColor());
      }
    }
  };

  virtual ~ElectronicComponent() = default;
};

class Wire {
private:
  Pin *pin0 = nullptr;
  Pin *pin1 = nullptr;

public:
  Wire(Pin *a, Pin *b) : pin0(a), pin1(b) {}

  Pin *getPin(int index) const { return (index == 0) ? pin0 : pin1; }

  Pin *other(const Pin *p) const {
    if (p == pin0)
      return pin1;
    if (p == pin1)
      return pin0;
    return nullptr;
  }

  void draw() const {
    if (!pin0 || !pin1)
      return;

    Vector2 a = pin0->getCenterPosition();
    Vector2 b = pin1->getCenterPosition();
    Color wireColor = pin1->getColor();

    DrawLineEx(a, b, 8.0f * safeScreenScale, BLACK);     // Outline
    DrawLineEx(a, b, 6.0f * safeScreenScale, wireColor); // Wire
  }
};

#endif
