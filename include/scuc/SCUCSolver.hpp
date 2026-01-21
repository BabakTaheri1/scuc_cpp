#pragma once

#include <memory>

#include "scuc/Config.hpp"
#include "scuc/Types.hpp"
#include "scuc/VariableManager.hpp"


namespace operations_research {
class MPSolver;
}


namespace scuc {


/**
 * @brief High-level orchestrator for SCUC.
 *
 * Owns:
 *  - loaded input data
 *  - OR-Tools solver instance
 *  - variable manager
 *  - objective / constraints builders
 *
 * Keeps the solve workflow simple:
 *   Load -> Build vars -> Build obj -> Build constraints -> Solve -> Export
 */
class SCUCSolver {
 public:
  explicit SCUCSolver(ModelConfig cfg);

  bool Solve();

  double ObjectiveValue() const { return objective_value_; }
  const UCInputData& Input() const { return uc_; }
  const VariableManager& Vars() const { return vars_; }

 private:
  ModelConfig cfg_;
  UCInputData uc_;

  std::unique_ptr<operations_research::MPSolver> solver_;
  VariableManager vars_;
  double objective_value_ = 0.0;
};

}  // namespace scuc
