#pragma once

#include "scuc/Config.hpp"
#include "scuc/Types.hpp"
#include "scuc/VariableManager.hpp"

#include "ortools/linear_solver/linear_solver.h"

namespace scuc {

/**
 * @brief Adds objective terms (minimization) to the OR-Tools model.
 *
 * Separation keeps the solver orchestration clean and makes it easy to experiment
 * with alternative objective formulations.
 */
class ObjectiveBuilder {
 public:
  void Build(operations_research::MPSolver& solver,
             const ModelConfig& cfg,
             const UCInputData& uc,
             const VariableManager& vars) const;
};

}  // namespace scuc
