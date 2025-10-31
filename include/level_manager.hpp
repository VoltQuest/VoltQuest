#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include "../include/game_objects/electronic_components/active_components.hpp"
#include "../include/game_objects/electronic_components/electronics_base.hpp"
#include "../include/game_objects/electronic_components/power_sources.hpp"
#include "../include/game_objects/electronic_components/wire.hpp"
#include "game_objects/movable_object.hpp"
#include "raylib.h"
#include "ui_manager.hpp"
#include <memory>
#include <vector>
namespace ElectronicsLevel {
// Global variables
void processLevel();
void resetLevel();
void loadTextures();
void updateLevel();
void handleClick();
void drawLevel();
void drawComponentsPanel(
    std::vector<std::shared_ptr<ElectronicComponent>> &objects,
    std::shared_ptr<ElectronicComponent> &activeObject,
    std::vector<Wire> &wires, bool &isPlacingWire,
    std::shared_ptr<ElectronicComponent> &wireStartObject);
} // namespace ElectronicsLevel

#endif
