#pragma once

#include <string>

#include "scuc/Config.hpp"
#include "scuc/Types.hpp"

namespace scuc {

/**
 * @brief Responsible for reading JSON input and producing a fully-populated UCInputData.
 *
 * This includes:
 *  - Buses/lines/generators/loads/reserves parsing
 *  - nodal + system demand computation
 *  - PTDF/LODF computation (NetworkFactors)
 *  - contingency line extraction + relevant N-1 pair preprocessing
 */
class InputLoader {
 public:
  UCInputData LoadFromFile(const std::string& path, const ModelConfig& cfg) const;

 private:
  void PreprocessContingencies(UCInputData& uc) const;
};

}  // namespace scuc
