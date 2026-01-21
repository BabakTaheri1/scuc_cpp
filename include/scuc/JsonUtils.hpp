#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace scuc {

/**
 * @brief Helpers used by InputLoader and CLI parsing.
 */
bool IsNumber(const nlohmann::json& j);

/**
 * @brief Read a time-series parameter that can be:
 *  - null    => zeros
 *  - number  => constant over time
 *  - array   => length must equal T
 */
std::vector<double> GetTimeSeries(const nlohmann::json& j, int T, const std::string& name);

/**
 * @brief Parse optional args like "--solver=CBC" from argv.
 */
std::optional<std::string> GetArgValue(int argc, char** argv, const std::string& key);

}  // namespace scuc
