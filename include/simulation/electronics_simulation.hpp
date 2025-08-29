#ifndef ELECTRONICS_SIMULATION_HPP
#define ELECTRONICS_SIMULATION_HPP

#include "../game_objects/electronic_components/electronics_base.hpp"
#include "../game_objects/electronic_components/wire.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// ---------- Net data ----------
struct Net {
  int id = -1;
  float voltage = 0.0f;
  bool voltage_set = false;
  std::unordered_set<Pin *> pins;
};

// --- helpers ---
static inline int findNetIndexForPin(Pin *pin, const std::vector<Net> &nets) {
  for (int i = 0; i < static_cast<int>(nets.size()); ++i) {
    if (nets[i].pins.count(pin))
      return i;
  }
  return -1;
}

// Build a netlist: each wire merges the nets of its two endpoint pins.
static std::vector<Net>
buildNetlist(std::vector<std::shared_ptr<ElectronicComponent>> &components,
             std::vector<Wire> &wires) {
  std::vector<Net> nets;
  nets.reserve(64);

  std::unordered_map<Pin *, int> pin_to_net;
  pin_to_net.reserve(128);

  int net_id = 0;
  for (auto &comp : components) {
    for (auto &pin : comp->pins) {
      Net net;
      net.id = net_id;
      net.pins.insert(&pin);
      nets.push_back(net);
      pin_to_net[&pin] = net_id;
      ++net_id;
    }
  }

  auto merge_nets = [&](int i, int j) {
    if (i == j) {
      return i;
    }
    int larger_net = (nets[i].pins.size() >= nets[j].pins.size()) ? i : j;
    int smaller_net = (larger_net == i) ? j : i;

    for (auto *p : nets[smaller_net].pins) {
      nets[larger_net].pins.insert(p);
      pin_to_net[p] = larger_net;
    }
    // keep any absolute voltage only if destination isn't set yet
    if (nets[smaller_net].voltage_set && !nets[larger_net].voltage_set) {
      nets[larger_net].voltage = nets[smaller_net].voltage;
      nets[larger_net].voltage_set = true;
    }
    nets[smaller_net].pins.clear();
    return larger_net;
  };

  for (auto &w : wires) {
    Pin *start = w.start_pin;
    Pin *end = w.end_pin;
    if (!start || !end)
      continue;

    int net_start = pin_to_net[start];
    int net_end = pin_to_net[end];
    if (net_start != net_end)
      merge_nets(net_start, net_end);
  }

  std::vector<Net> compact;
  compact.reserve(nets.size());
  for (auto &net : nets) {
    if (!net.pins.empty())
      compact.push_back(std::move(net));
  }
  for (int i = 0; i < (int)compact.size(); ++i)
    compact[i].id = i;

  return compact;
}

// ---------- Constraint graph (for ideal voltage sources) ----------
struct Edge {
  int to;      // neighbor net index
  float delta; // V_to = V_from + delta
};

// Apply sources by building a constraint graph: each battery says
// V(pos) = V(neg) + Vbat. Ground reference is established first.
static void
applySources(std::vector<std::shared_ptr<ElectronicComponent>> &components,
             std::vector<Net> &nets) {

  const float EPS = 1e-3f;
  std::vector<std::vector<Edge>> adj(nets.size());

  auto add_edge = [&](int from, int to, float d) {
    if (from >= 0 && to >= 0)
      adj[from].push_back({to, d});
  };

  // Build constraints and find ground candidates
  std::vector<int> candidate_grounds;
  for (auto &comp : components) {
    if (comp->label == ComponentLabel::Battery && comp->pins.size() >= 2) {
      Pin *posPin = &comp->pins[0];
      Pin *negPin = &comp->pins[1];
      int npos = findNetIndexForPin(posPin, nets);
      int nneg = findNetIndexForPin(negPin, nets);

      // Add bidirectional constraints: V_pos = V_neg + voltage
      add_edge(nneg, npos, comp->voltage);
      add_edge(npos, nneg, -comp->voltage);

      if (nneg >= 0)
        candidate_grounds.push_back(nneg);
    }
  }

  // --- Choose ground reference ---
  std::vector<int> seed_nodes;
  if (!candidate_grounds.empty()) {
    // Use the first battery's negative terminal as ground
    int ground_net = candidate_grounds[0];
    nets[ground_net].voltage = 0.0f;
    nets[ground_net].voltage_set = true;
    seed_nodes.push_back(ground_net);
  } else if (!nets.empty()) {
    // No batteries: pick net 0 as reference
    nets[0].voltage = 0.0f;
    nets[0].voltage_set = true;
    seed_nodes.push_back(0);
  }

  // Propagate constraints using BFS
  std::queue<int> q;
  for (int s : seed_nodes)
    q.push(s);

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    float Vu = nets[u].voltage;

    for (const auto &e : adj[u]) {
      int v = e.to;
      float candidate_voltage = Vu + e.delta;

      if (!nets[v].voltage_set) {
        nets[v].voltage = candidate_voltage;
        nets[v].voltage_set = true;
        q.push(v);
      } else if (std::fabs(nets[v].voltage - candidate_voltage) > EPS) {
        // Voltage conflict detected - keep first assignment for now
      }
    }
  }
}

static float getPinVoltage(Pin *pin, const std::vector<Net> &nets) {
  for (const auto &net : nets) {
    if (net.pins.count(pin))
      return net.voltage_set ? net.voltage : 0.0f;
  }
  return 0.0f;
}

// Check if a pin is connected to any other component (has a complete path)
static bool isPinInCompleteCircuit(Pin *pin, const std::vector<Net> &nets) {
  // Find the net containing this pin
  for (const auto &net : nets) {
    if (net.pins.count(pin)) {
      // A pin is in a complete circuit if:
      // 1. Its net has a set voltage (connected to a voltage source path)
      // 2. The net contains pins from different components (not isolated)
      return net.voltage_set && net.pins.size() > 1;
    }
  }
  return false;
}

// Check if both pins of a component are in complete circuits
static bool isComponentInCompleteCircuit(Pin *pin1, Pin *pin2,
                                         const std::vector<Net> &nets) {
  return isPinInCompleteCircuit(pin1, nets) &&
         isPinInCompleteCircuit(pin2, nets);
}

// Update components using their own parameters
static void
updateComponents(std::vector<std::shared_ptr<ElectronicComponent>> &components,
                 const std::vector<Net> &nets) {
  for (auto &comp : components) {
    switch (comp->label) {
    case ComponentLabel::Led: {
      if (comp->pins.size() >= 2) {
        Pin *anode = &comp->pins[0];
        Pin *cathode = &comp->pins[1];

        float Va = getPinVoltage(anode, nets);
        float Vc = getPinVoltage(cathode, nets);
        float forward_voltage = comp->voltage;
        float voltage_drop = Va - Vc;

        bool in_complete_circuit =
            isComponentInCompleteCircuit(anode, cathode, nets);

        // LED behavior based on voltage drop across it AND circuit completeness
        const float tolerance = 1.0f; // Allow some tolerance for "on" state

        // LED is on if:
        // 1. It's in a complete circuit
        // 2. Voltage drop is sufficient but not excessive
        // 3. Both pins have different voltages (current can flow)
        bool sufficient_voltage =
            (voltage_drop >= forward_voltage) &&
            (voltage_drop <= (forward_voltage + tolerance));
        bool has_current_path =
            std::fabs(voltage_drop) > 0.001f; // Not the same voltage

        bool led_on =
            in_complete_circuit && sufficient_voltage && has_current_path;

        // LED is damaged if voltage drop is too high AND it's in a complete
        // circuit
        bool led_damaged = in_complete_circuit &&
                           (voltage_drop > (forward_voltage + tolerance));

        comp->powered = led_on;
        comp->damaged = led_damaged;
      }
    } break;

    default:
      break;
    }
  }
}

inline void
simulateLevel(std::vector<std::shared_ptr<ElectronicComponent>> &components,
              std::vector<Wire> &wires) {

  // Build netlist
  auto nets = buildNetlist(components, wires);

  // Apply voltage sources
  applySources(components, nets);

  // Debug: Print net voltages
  for (const auto &net : nets) {
    if (net.voltage_set) {
    } else {
    }
  }

  // Update component states
  updateComponents(components, nets);

  // Update pin voltages
  for (auto &net : nets) {
    for (auto *p : net.pins) {
      if (net.voltage_set)
        p->voltage = net.voltage;
    }
  }
}

#endif // ELECTRONICS_SIMULATION_HPP
