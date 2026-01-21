#pragma once

#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace scuc {

/** Piecewise linear cost segment: p in [0, length], cost slope $/MW */
struct CostSegment {
  double length = 0.0;
  double slope  = 0.0;
};

/** Startup category: cost depends on downtime ("delay" hours since last ON). */
struct StartupStage {
  int delay = 0;      // hours since last ON
  double cost = 0.0;  // $
};

struct ThermalGenData {
  std::string name;
  std::string bus;

  double p_min = 0.0;
  double p_max = 0.0;
  double ramp_up = 9999.0;
  double ramp_down = 9999.0;
  double startup_limit = 9999.0;
  double shutdown_limit = 9999.0;
  int min_uptime = 1;
  int min_downtime = 1;
  double no_load_cost = 0.0;

  bool must_run = false;
  bool reserve_eligible = false;

  // Positive => ON for that many hours; Negative => OFF for that many hours.
  int initial_status = 0;
  double initial_power = 0.0;

  std::vector<CostSegment> cost_segments;
  std::vector<StartupStage> startup_stages;

  // Optional fixed commitment schedule (nullopt => free)
  std::vector<std::optional<int>> commitment_status;
};

struct ProfiledGenData {
  std::string name;
  std::string bus;
  std::vector<double> p_min_ts;
  std::vector<double> p_max_ts;
  double cost_per_mwh = 0.0;
};

struct PriceSensitiveLoad {
  std::string name;
  std::string bus;
  std::vector<double> demand_ts;
  std::vector<double> revenue_ts;
};

struct ReserveRequirement {
  std::string name;
  std::vector<double> amount_ts;
  double penalty = -1.0; // shortfall penalty $/MW; <=0 disables shortfall var in objective
};

struct LineData {
  std::string from_bus;
  std::string to_bus;

  // At least one must exist:
  std::optional<double> reactance;    // ohms
  std::optional<double> susceptance;  // Siemens

  double normal_limit    = std::numeric_limits<double>::quiet_NaN();
  double emergency_limit = std::numeric_limits<double>::quiet_NaN();
  double flow_penalty    = std::numeric_limits<double>::quiet_NaN();
};

/** k = outaged line index, l = monitored line index */
struct ContingencyPair {
  int k = -1;
  int l = -1;
};

/**
 * @brief Unified container for everything the model needs.
 *
 * After loading, this struct is treated as read-only by the model builders.
 */
struct UCInputData {
  int T = 0;
  std::string ref_bus;

  std::vector<std::string> buses;
  std::unordered_map<std::string, int> bus_map;

  std::vector<std::string> lines;
  std::unordered_map<std::string, LineData> line_data;
  std::unordered_map<std::string, int> line_map;

  std::vector<ThermalGenData> thermal_gens;
  std::vector<ProfiledGenData> profiled_gens;
  std::vector<PriceSensitiveLoad> psl;
  std::vector<ReserveRequirement> reserves;

  // Demand
  std::vector<double> system_demand;                 // [t]
  std::vector<std::vector<double>> nodal_demand;     // [t][b]

  // Factors
  std::vector<std::vector<double>> PTDF; // [l][b]
  std::vector<std::vector<double>> LODF; // [l][k]

  // Contingency lines from JSON (line names)
  std::vector<std::string> contingency_lines;

  // Preprocessed N-1 pairs used by the model
  std::vector<ContingencyPair> relevant_pairs;

  double PTDF_at(int l, int b) const { return PTDF.at(l).at(b); }
  double LODF_at(int l, int k) const { return LODF.at(l).at(k); }
};

}  // namespace scuc
