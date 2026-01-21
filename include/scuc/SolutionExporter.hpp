#pragma once

#include <string>

#include "scuc/Types.hpp"
#include "scuc/VariableManager.hpp"

namespace scuc {

/**
 * @brief Writes user-facing outputs (report, JSON, CSV).
 *
 * Keeping IO out of SCUCSolver reduces coupling and keeps the model code clean.
 */
class SolutionExporter {
 public:
  void Report(const UCInputData& uc, const VariableManager& vars, double objective_value) const;

  void SaveSolutionToJson(const UCInputData& uc,
                          const VariableManager& vars,
                          double objective_value,
                          const std::string& out_path) const;

  void SaveDispatchCsv(const UCInputData& uc,
                       const VariableManager& vars,
                       const std::string& out_path) const;
};

}  // namespace scuc
