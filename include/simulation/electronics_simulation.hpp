#ifndef ELECTRONICS_SIMULATION_HPP
#define ELECTRONICS_SIMULATION_HPP

#include "../game_objects/electronic_components/electronics_base.hpp"
#include <memory>
#include <vector>

class ElectronicsSimulation {
public:
  // Build / rebuild topology (expensive)
  void build(const std::vector<std::shared_ptr<ElectronicComponent>> &objects,
             const std::vector<Wire> &node_edges);

  // Solve using cached topology (cheap, repeatable)
  void solve();

  // Explicit invalidation hook
  void markDirty() { dirty = true; }

private:
  // ---- Cached topology (valid only when !dirty) ----
  std::vector<Pin *> all_pins; // non-owning
  bool dirty = true;

  // ---- Internal pipeline ----
  void clearCache();
  void
  fetchPins(const std::vector<std::shared_ptr<ElectronicComponent>> &objects);
  void buildNodes(const std::vector<Wire> &node_edges);
  void stampMatrix();
};

#endif // ELECTRONICS_SIMULATION_HPP
