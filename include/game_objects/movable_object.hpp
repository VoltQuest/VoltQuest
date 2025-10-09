#ifndef MOVABLE_OBJECT_HPP
#define MOVABLE_OBJECT_HPP

#include "raylib.h"

struct MovableObject {
  Vector2 position;
  float rotation;
  bool is_active = false;
  bool is_dragged = false;

  MovableObject(Vector2 pos = {0, 0}, float rotate = 0.0f)
      : position(pos), rotation(rotate) {}

  virtual void update() = 0;
  virtual void draw() = 0;

  virtual Rectangle getCollider() const = 0;
  virtual ~MovableObject() = default;
};

#endif
