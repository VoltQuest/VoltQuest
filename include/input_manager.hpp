#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "ui_utils.hpp"
#include <raylib.h>

struct VoltInput;
inline VoltInput *activeDrag = nullptr;

struct VoltInput {
  bool dragging = false;
  Vector2 dragOffset = {0, 0};
  int dragTouchId = -1;

  void mouseDragRect(Rectangle &collision, Vector2 &position,
                     bool &is_active_selected) {
    Vector2 inputPos = {0, 0};
    bool inputDown = false;
    bool inputPressed = false;
    bool inputReleased = false;

    // --- Mouse Handling ---
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

    // --- Touch Handling ---
    int touchCount = GetTouchPointCount();
    if (touchCount > 0) {
      if (dragTouchId == -1) {
        // No touch active; look for a new one
        for (int i = 0; i < touchCount; i++) {
          Vector2 tp = GetTouchPosition(i);
          if (CheckCollisionPointRec(tp, collision)) {
            inputPos = tp;
            inputPressed = true;
            inputDown = true;
            dragTouchId = GetTouchPointId(i);
            break;
          }
        }
      } else {
        // Existing touch is active
        bool found = false;
        for (int i = 0; i < touchCount; i++) {
          if (GetTouchPointId(i) == dragTouchId) {
            inputPos = GetTouchPosition(i);
            inputDown = true;
            found = true;
            break;
          }
        }

        if (!found) {
          // Finger was lifted
          inputReleased = true;
          dragTouchId = -1;
        }
      }
    }

    // --- Start Dragging ---
    if (inputPressed && CheckCollisionPointRec(inputPos, collision)) {
      if (activeDrag == nullptr) {
        dragOffset = {inputPos.x - position.x, inputPos.y - position.y};
        dragging = true;
        activeDrag = this;
        is_active_selected = true;
      }
    }

    // --- Stop Dragging ---
    if (inputReleased && dragging) {
      dragging = false;
      if (activeDrag == this) {
        activeDrag = nullptr;
        is_active_selected = false;
      }
    }

    // --- Move Only If Still Dragging ---
    if (dragging && inputDown && activeDrag == this) {
      position = {inputPos.x - dragOffset.x, inputPos.y - dragOffset.y};
    }
  }
};

#endif
