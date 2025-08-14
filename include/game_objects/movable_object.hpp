#ifndef MOVABLE_OBJECT_HPP
#define MOVABLE_OBJECT_HPP

#include "raylib.h"

struct MovableObject {
  Vector2 position;
  float rotation;
  bool is_active = false;
  Texture2D texture;

  MovableObject(Vector2 pos = {0, 0}, float rotate = 0.0f)
      : position(pos), rotation(rotate) {}

  virtual void loadObjectTexture() = 0;
  virtual void update() = 0;
  virtual void draw() = 0;
  virtual void unloadObjectTexture() = 0;

  virtual Rectangle getCollider() const = 0;
  virtual ~MovableObject() = default;
};

#endif
