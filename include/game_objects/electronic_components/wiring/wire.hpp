#ifndef WIRE_HPP
#define WIRE_HPP

#include "../electronics_base.hpp"
#include "raylib.h"

struct Wire {
  Pin *start_pin = nullptr;
  Pin *end_pin = nullptr;

  Wire(Pin *from, Pin *to) : start_pin(from), end_pin(to) {}

  void draw() const {
    if (!start_pin || !end_pin)
      return;

    Vector2 start = start_pin->getCenterPosition();
    Vector2 end = end_pin->getCenterPosition();
    Color wireColor = end_pin->color;

    DrawLineEx(start, end, 6.0f, BLACK);     // Outline
    DrawLineEx(start, end, 4.0f, wireColor); // Colored wire
  }
};

#endif
