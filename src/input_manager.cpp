#include "../include/input_manager.hpp"
#include <unordered_map>

namespace InputManager {
// ───── PRIVATE STATE ─────
static Vector2 internal_mouse_pos = {0, 0};
static MovableObject *active_selection = nullptr; // Renamed from activeDrag

// ───── PUBLIC INTERFACE ─────

Vector2 GetCachedMousePos() { return internal_mouse_pos; }

MovableObject *GetActiveSelection() { return active_selection; }

void ClearActiveSelection() { active_selection = nullptr; }

void updateMousePos() { internal_mouse_pos = GetMousePosition(); }

void updateDragInputs(MovableObject &gameObject) {
  Vector2 inputPos = {0, 0};
  bool inputDown = false;
  bool inputPressed = false;
  bool inputReleased = false;

  static std::unordered_map<MovableObject *, Vector2> dragOffsets;
  static std::unordered_map<MovableObject *, int> dragTouchIds;

  // ... [Mouse/Touch logic remains the same] ...
  // (Copy the Mouse/Touch detection block from previous steps)
  // For brevity, assuming standard detection logic here:
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    inputPos = GetMousePosition();
    inputPressed = true;
    inputDown = true;
  } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    inputPos = GetMousePosition();
    inputDown = true;
  } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    inputReleased = true;
  }

  Rectangle collision = gameObject.getCollider();

  // ───── LOGIC USING NEW VARIABLE NAME ─────

  // --- Start Dragging ---
  if (inputPressed && CheckCollisionPointRec(inputPos, collision)) {
    // Only start if nothing is currently selected
    if (active_selection == nullptr) {
      dragOffsets[&gameObject] = {inputPos.x - gameObject.position.x,
                                  inputPos.y - gameObject.position.y};
      gameObject.is_dragged = true;
      gameObject.is_active = true;

      // Set the private variable
      active_selection = &gameObject;
    }
  }

  // --- Stop Dragging ---
  if (inputReleased && gameObject.is_dragged) {
    gameObject.is_dragged = false;

    // Only clear if THIS object was the one selected
    if (active_selection == &gameObject) {
      active_selection = nullptr;
    }
  }

  // --- Move Only If Still Dragging ---
  if (gameObject.is_dragged && inputDown && active_selection == &gameObject) {
    gameObject.position = {inputPos.x - dragOffsets[&gameObject].x,
                           inputPos.y - dragOffsets[&gameObject].y};
  }
}
} // namespace InputManager
