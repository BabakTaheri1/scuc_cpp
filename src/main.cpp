#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

#include "scuc/Config.hpp"
#include "scuc/JsonUtils.hpp"
#include "scuc/SCUCSolver.hpp"
#include "scuc/SolutionExporter.hpp"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: " << argv[0] << " <path/to/input.json> [--solver=CBC] [--time_limit=600]\n";
      return 2;
    }

    scuc::ModelConfig cfg;
    cfg.input_data_path = argv[1];

    if (auto s = scuc::GetArgValue(argc, argv, "solver")) cfg.solver_name = *s;
    if (auto tl = scuc::GetArgValue(argc, argv, "time_limit")) cfg.solver_time_limit_sec = std::stoi(*tl);

    // Put results next to a case folder name for neat organization.
    const fs::path input_path(cfg.input_data_path);
    const std::string case_name = input_path.parent_path().filename().string();

    const fs::path results_dir = fs::path("results") / case_name;
    fs::create_directories(results_dir);

    cfg.solution_output_filename = (results_dir / "solution_output.json").string();
    cfg.dispatch_csv_filename    = (results_dir / "dispatch.csv").string();

    scuc::SCUCSolver solver(cfg);
    if (!solver.Solve()) return 1;

    scuc::SolutionExporter out;
    out.SaveSolutionToJson(solver.Input(), solver.Vars(), solver.ObjectiveValue(), cfg.solution_output_filename);
    out.SaveDispatchCsv(solver.Input(), solver.Vars(), cfg.dispatch_csv_filename);

    // Optional console summary (comment out if you want silent runs)
    out.Report(solver.Input(), solver.Vars(), solver.ObjectiveValue());

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << "\n";
    return 1;
  }
}
