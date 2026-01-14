#ifndef ELECTRONICS_BASE_HPP
#define ELECTRONICS_BASE_HPP
#include "../../ui_utils.hpp"
#include "../movable_object.hpp"
#include "raylib.h"
#include <cstdint>
#include <vector>

enum class PinType : uint16_t { Power, Ground, Input, Output, BiDirectional };

enum class ComponentLabel : uint16_t {
  Battery = 0,
  Led = 1,
  Resistor = 2,
  Switch = 3,
};

struct Pin {
  Vector2 relative_position;
  Rectangle collider;
  float colliderSize = 16.0f * safeScreenScale;
  PinType type;
  Color color;
  float voltage = 0.0f;
  float current = 0.0f;
  bool is_connected = false;
  int index = -1;
  int node_id = -1;

  Pin(Vector2 relPos, PinType pinType)
      : relative_position(relPos), type(pinType) {
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
    collider.x = componentPos.x + relative_position.x - (colliderSize / 2.0f);
    collider.y = componentPos.y + relative_position.y - (colliderSize / 2.0f);
  }

  bool isHovered() const {
    return (CheckCollisionPointRec(GetMousePosition(), collider));
  }

  Vector2 getCenterPosition() const {
    return {collider.x + colliderSize / 2.0f, collider.y + colliderSize / 2.0f};
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
        DrawRectangle(pin.collider.x, pin.collider.y, pin.colliderSize,
                      pin.colliderSize, pin.color);
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
    Color wireColor = pin1->color;

    DrawLineEx(a, b, 8.0f * safeScreenScale, BLACK);     // Outline
    DrawLineEx(a, b, 6.0f * safeScreenScale, wireColor); // Wire
  }
};

#endif
