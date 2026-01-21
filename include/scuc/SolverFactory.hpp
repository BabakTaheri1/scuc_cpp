#pragma once

#include <memory>
#include <string>

#include "scuc/Config.hpp"

#include "ortools/linear_solver/linear_solver.h"

namespace scuc {

/** Map string to OR-Tools MPSolver type. */
operations_research::MPSolver::OptimizationProblemType
SolverTypeFromName(const std::string& name);

/**
 * @brief Create an OR-Tools solver and apply generic settings (time limit, etc.).
 *
 * Note: Some backends (CBC in your build) do not accept solver-specific param strings.
 * We intentionally avoid calling SetSolverSpecificParametersAsString() for CBC to avoid warnings.
 */
std::unique_ptr<operations_research::MPSolver> CreateSolverOrDie(const ModelConfig& cfg);

}  // namespace scuc
