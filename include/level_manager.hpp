#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include "../include/game_objects/electronic_components/electronics_base.hpp"
#include "raylib.h"
#include "ui_manager.hpp"
#include <memory>
#include <vector>

class ElectronicsLevel {
private:
  std::vector<std::shared_ptr<ElectronicComponent>> objects;
  std::vector<Wire> wires;

  std::shared_ptr<ElectronicComponent> activeObject;
  bool is_placing_wire = false;
  std::shared_ptr<ElectronicComponent> wireStartObject = nullptr;
  Pin *wireStartPin = nullptr;

public:
  ElectronicsLevel();
  ~ElectronicsLevel();
  void processLevel();
  void resetLevel();
  void loadTextures();
  void updateLevel();
  void drawLevel();
  void drawComponentsPanel();
};

#endif
