#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#define INPUT_MANAGER_HPP
#include "game_objects/movable_object.hpp"
#include "raylib.h"

namespace InputManager {
Vector2 GetCachedMousePos();
MovableObject *GetActiveSelection();

void ClearActiveSelection();
void updateMousePos();
void updateDragInputs(MovableObject &gameObject);
} // namespace InputManager

#endif
