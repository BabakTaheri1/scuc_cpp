#pragma once

#include "scuc/Config.hpp"
#include "scuc/Types.hpp"

namespace scuc {

/**
 * @brief Compute PTDF and LODF matrices and store them into uc.PTDF / uc.LODF.
 *
 * Uses DC approximation:
 *   B_bus = A * diag(b_line) * A^T
 * then removes the ref bus and solves for inverse via LDLT.
 */
void CalculateNetworkFactors(UCInputData& uc, const ModelConfig& cfg);

}  // namespace scuc
