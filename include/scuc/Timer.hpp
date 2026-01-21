#pragma once

#include <chrono>
#include <string>

namespace scuc {

/**
 * @brief Simple RAII timer that prints elapsed time when it goes out of scope.
 */
class Timer {
 public:
  explicit Timer(std::string name);
  ~Timer();

  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

 private:
  std::string name_;
  std::chrono::high_resolution_clock::time_point start_;
};

}  // namespace scuc
