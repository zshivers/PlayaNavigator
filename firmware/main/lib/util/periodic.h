#pragma once

#include <cstdint>

class Periodic {
 public:
  explicit Periodic(uint32_t period_ms) : period_ms_(period_ms), next_ms_(0) {}
  bool IsDue(uint32_t ms) {
    if (ms >= next_ms_) {
      next_ms_ = ms + period_ms_;
      return true;
    }
    return false;
  }

 private:
  const uint32_t period_ms_;
  uint32_t next_ms_;
};