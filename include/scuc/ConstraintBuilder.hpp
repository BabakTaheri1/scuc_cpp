#pragma once

#include "scuc/Config.hpp"
#include "scuc/Types.hpp"
#include "scuc/VariableManager.hpp"

#include "ortools/linear_solver/linear_solver.h"

namespace scuc {

/**
 * @brief Adds all constraints to the optimization model.
 *
 * We keep this separate because constraints grow quickly in real SCUC engines.
 * This design keeps the "business logic" readable and testable.
 */
class ConstraintBuilder {
 public:
  void Build(operations_research::MPSolver& solver,
             const ModelConfig& cfg,
             const UCInputData& uc,
             const VariableManager& vars) const;

 private:
  // System-level constraints
  void AddSystemConstraints(operations_research::MPSolver& solver,
                            const UCInputData& uc,
                            const VariableManager& vars) const;

  // Thermal + profiled + PSL constraints
  void AddGeneratorConstraints(operations_research::MPSolver& solver,
                               const UCInputData& uc,
                               const VariableManager& vars) const;

  // DC network constraints (base + N-1)
  void AddNetworkConstraints(operations_research::MPSolver& solver,
                             const ModelConfig& cfg,
                             const UCInputData& uc,
                             const VariableManager& vars) const;

  // Helpers for thermal modeling
  void AddStartupSumCoeff(operations_research::MPConstraint* c,
                          const UCInputData& uc,
                          const VariableManager& vars,
                          int gi, int t, double coef) const;

  void AddProdAboveCoeff(operations_research::MPConstraint* c,
                         const UCInputData& uc,
                         const VariableManager& vars,
                         int gi, int t, double coef) const;

  void AddInitialFixConstraints(operations_research::MPSolver& solver,
                                const UCInputData& uc,
                                const VariableManager& vars,
                                int gi) const;

  void AddMinUpDownCumulative(operations_research::MPSolver& solver,
                              const UCInputData& uc,
                              const VariableManager& vars,
                              int gi) const;

  void AddStartupCategoryRestrict(operations_research::MPSolver& solver,
                                  const UCInputData& uc,
                                  const VariableManager& vars,
                                  int gi) const;
};

}  // namespace scuc
