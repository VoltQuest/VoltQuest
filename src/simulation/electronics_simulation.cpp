#include "../../include/simulation/electronics_simulation.hpp"
#include "../../include/game_objects/electronic_components/electronics_base.hpp"
#include <cassert>
#include <memory>
#include <vector>

void ElectronicsSimulation::clearCache() { all_pins.clear(); }

void ElectronicsSimulation::build(
    const std::vector<std::shared_ptr<ElectronicComponent>> &objects,
    const std::vector<Wire> &node_edges) {
  clearCache();

  fetchPins(objects);
  buildNodes(node_edges);
  stampMatrix();

  dirty = false;
}

void ElectronicsSimulation::solve() {
  assert(!dirty && "solve() called before build()");

  // TODO:
  // - Solve linear system
  // - Write voltages / currents back into pins
}

void ElectronicsSimulation::fetchPins(
    const std::vector<std::shared_ptr<ElectronicComponent>> &objects) {
  for (const auto &obj : objects) {
    for (auto &pin : obj->pins) {
      all_pins.push_back(&pin);
    }
  }
}

void ElectronicsSimulation::buildNodes(const std::vector<Wire> &node_edges) {}

void ElectronicsSimulation::stampMatrix() {}
