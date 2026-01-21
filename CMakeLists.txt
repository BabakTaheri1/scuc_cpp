cmake_minimum_required(VERSION 3.20)

project(scuc
  VERSION 1.0
  DESCRIPTION "Security Constrained Unit Commitment (SCUC) Solver"
  LANGUAGES CXX
)

# ===========================
# Global C++ configuration
# ===========================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ===========================
# Dependencies
# ===========================
find_package(Eigen3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(ortools CONFIG REQUIRED)

# ===========================
# Modular sources
# ===========================
set(SCUC_MODULAR_SOURCES
  src/main.cpp
  src/Timer.cpp
  src/JsonUtils.cpp
  src/NetworkFactors.cpp
  src/SolverFactory.cpp
  src/InputLoader.cpp
  src/VariableManager.cpp
  src/ObjectiveBuilder.cpp
  src/ConstraintBuilder.cpp
  src/SolutionExporter.cpp
  src/SCUCSolver.cpp
)

# ===========================
# Targets
# ===========================

# Modular executable
add_executable(scuc_modular ${SCUC_MODULAR_SOURCES})
target_include_directories(scuc_modular PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(scuc_modular PRIVATE
  Eigen3::Eigen
  nlohmann_json::nlohmann_json
  ortools::ortools
)

# One-file executable (only if scuc3.cpp exists)
add_executable(scuc_onefile scuc3.cpp)
target_include_directories(scuc_onefile PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(scuc_onefile PRIVATE
  Eigen3::Eigen
  nlohmann_json::nlohmann_json
  ortools::ortools
)

# ===========================
# Optimization flags
# ===========================
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  if (NOT MSVC)
    target_compile_options(scuc_modular PRIVATE -O3)
    target_compile_options(scuc_onefile PRIVATE -O3)
  endif()
endif()

# ===========================
# RPATH for runtime linking
# ===========================
if(APPLE)
  set_target_properties(scuc_modular PROPERTIES BUILD_RPATH "@loader_path" INSTALL_RPATH "@loader_path")
  set_target_properties(scuc_onefile PROPERTIES BUILD_RPATH "@loader_path" INSTALL_RPATH "@loader_path")
elseif(UNIX)
  set_target_properties(scuc_modular PROPERTIES BUILD_RPATH "$ORIGIN" INSTALL_RPATH "$ORIGIN")
  set_target_properties(scuc_onefile PROPERTIES BUILD_RPATH "$ORIGIN" INSTALL_RPATH "$ORIGIN")
endif()

# ===========================
# Install rules (optional)
# ===========================
install(TARGETS scuc_modular scuc_onefile RUNTIME DESTINATION bin)

# ===========================
# Summary
# ===========================
message(STATUS "========================================")
message(STATUS " SCUC Solver Build Configuration")
message(STATUS "----------------------------------------")
message(STATUS " C++ Standard:        C++17")
message(STATUS " Build type:         ${CMAKE_BUILD_TYPE}")
message(STATUS " Targets:            scuc_modular, scuc_onefile")
message(STATUS "========================================")
