#pragma once

#include <string>

namespace scuc {

/**
 * @brief Runtime configuration for the SCUC model and solver.
 *
 * Keep this small and focused: CLI / main populates it, other components read it.
 */
struct ModelConfig {
  std::string input_data_path;

  std::string solution_output_filename = "solution_output.json";
  std::string dispatch_csv_filename    = "dispatch.csv";

  int solver_time_limit_sec = 600;
  std::string solver_name = "CBC";  // CBC, SCIP, GUROBI, CPLEX, GLPK, HIGHS (if available)

  // Network defaults if limits/penalties are missing from JSON:
  double default_line_limit_mw = 10000.0;
  double default_flow_penalty  = 10000.0;

  // Load curtailment penalty ($/MWh). Large number forces serving demand.
  double curtail_penalty = 100000.0;

  // Factor sparsity (zero-out small PTDF/LODF entries)
  double ptdf_sparsity_cutoff = 0.01;
  double lodf_sparsity_cutoff = 0.05;

  // Requested relative optimality gap (best-effort; depends on backend support)
  double relative_gap = 0.01;
};

}  // namespace scuc
