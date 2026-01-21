#pragma once

#include <unordered_map>
#include <vector>

#include "scuc/Types.hpp"

#include "ortools/linear_solver/linear_solver.h"

namespace scuc {

/**
 * @brief Owns and indexes all OR-Tools decision variables.
 *
 * This class does NOT add constraints or objective terms.
 * It simply creates variables with consistent naming and dimensions.
 */
class VariableManager {
 public:
  VariableManager() = default;

  void Build(operations_research::MPSolver& solver, const UCInputData& uc);

  // Dimensions
  int T() const { return T_; }
  int G() const { return G_; }
  int P() const { return P_; }
  int PSL() const { return PSL_; }
  int L() const { return L_; }
  int NB() const { return NB_; }
  int NR() const { return NR_; }

  // Index helpers (public for builders)
  int idx_g_t(int gi, int t) const { return gi * T_ + t; }
  int idx_b_t(int b, int t) const { return b * T_ + t; }
  int idx_l_t(int l, int t) const { return l * T_ + t; }
  int idx_p_t(int i, int t) const { return i * T_ + t; }
  int idx_stage_t(int s, int t) const { return s * T_ + t; }

  // Reserve mapping
  const std::vector<int>& reserve_gi() const { return reserve_gi_; }
  bool IsReserveEligible(int gi) const { return gi_to_resi_.count(gi) > 0; }
  int ReserveIndexOfGi(int gi) const { return gi_to_resi_.at(gi); }

  // Variables
  operations_research::MPVariable* p(int gi, int t) const { return p_.at(idx_g_t(gi, t)); }
  operations_research::MPVariable* u(int gi, int t) const { return u_.at(idx_g_t(gi, t)); }
  operations_research::MPVariable* w(int gi, int t) const { return w_.at(idx_g_t(gi, t)); }

  operations_research::MPVariable* curtail(int b, int t) const { return curtail_.at(idx_b_t(b, t)); }

  operations_research::MPVariable* r(int ri, int t) const { return r_.at(idx_p_t(ri, t)); }
  operations_research::MPVariable* reserve_shortfall(int rq, int t) const { return reserve_shortfall_.at(idx_p_t(rq, t)); }

  operations_research::MPVariable* p_prof(int pi, int t) const { return p_prof_.at(idx_p_t(pi, t)); }
  operations_research::MPVariable* psl_served(int li, int t) const { return psl_served_.at(idx_p_t(li, t)); }

  operations_research::MPVariable* viol_base(int l, int t) const { return viol_base_.at(idx_l_t(l, t)); }
  operations_research::MPVariable* viol_cont(int pair_i, int t) const { return viol_cont_.at(idx_p_t(pair_i, t)); }

  // Startup category binaries v[gi][s,t]
  operations_research::MPVariable* v(int gi, int s, int t) const { return v_.at(gi).at(idx_stage_t(s, t)); }

  // Piecewise segment p_seg[gi][s,t]
  operations_research::MPVariable* p_seg(int gi, int s, int t) const { return p_seg_.at(gi).at(idx_stage_t(s, t)); }

 private:
  // Cached dimensions
  int T_ = 0, G_ = 0, P_ = 0, PSL_ = 0, L_ = 0, NB_ = 0, NR_ = 0;

  // Reserve mappings
  std::vector<int> reserve_gi_;
  std::unordered_map<int, int> gi_to_resi_;

  // Variables
  std::vector<operations_research::MPVariable*> p_;
  std::vector<operations_research::MPVariable*> u_;
  std::vector<operations_research::MPVariable*> w_;
  std::vector<operations_research::MPVariable*> curtail_;

  std::vector<std::vector<operations_research::MPVariable*>> v_;
  std::vector<std::vector<operations_research::MPVariable*>> p_seg_;

  std::vector<operations_research::MPVariable*> r_;
  std::vector<operations_research::MPVariable*> reserve_shortfall_;

  std::vector<operations_research::MPVariable*> p_prof_;
  std::vector<operations_research::MPVariable*> psl_served_;

  std::vector<operations_research::MPVariable*> viol_base_;
  std::vector<operations_research::MPVariable*> viol_cont_;
};

}  // namespace scuc
