#ifndef ELECTRONICS_BASE_HPP
#define ELECTRONICS_BASE_HPP
#include "../movable_object.hpp"
#include "raylib.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

enum class PinType : uint16_t { Power, Ground, Input, Output, BiDirectional };

enum class ComponentLabel : uint16_t {
  Battery = 0,
  Led = 1,
  Resistor = 2,
  Switch = 3,
};

inline const std::unordered_map<ComponentLabel, std::string> ComponentNames{
    {ComponentLabel::Battery, "Battery"},
    {ComponentLabel::Led, "Led"},
    {ComponentLabel::Resistor, "Resistor"},
    {ComponentLabel::Switch, "Switch"}};

inline const std::string &GetComponentName(ComponentLabel type) {
  static const std::string unknown = "Unknown";
  auto it = ComponentNames.find(type);
  return (it != ComponentNames.end()) ? it->second : unknown;
}

struct Pin {
  Vector2 relative_position;
  Rectangle collider;
  PinType type;
  Color color;
  float voltage = 0.0f;
  float current = 0.0f;
  bool is_connected = false;

  Pin(Vector2 relPos, PinType pinType, float colliderSize = 18.0f)
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
    collider.x = componentPos.x + relative_position.x;
    collider.y = componentPos.y + relative_position.y;
  }

  bool isHovered() const {
    return (CheckCollisionPointRec(GetMousePosition(), collider));
  }

  Vector2 getCenterPosition() const {
    return {collider.x + collider.width / 2.0f,
            collider.y + collider.height / 2.0f};
  }
};

struct ElectronicComponent : public MovableObject {
  float voltage;
  float current;
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
        DrawRectangle(pin.collider.x, pin.collider.y, pin.collider.width,
                      pin.collider.height, pin.color);
      }
    }
  };

  virtual ~ElectronicComponent() = default;
};

#endif
