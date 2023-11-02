#include "hal_time.h"

#include <chrono>
#include <iostream>

namespace {
using hr_clock = std::chrono::high_resolution_clock;
typedef std::chrono::duration<float, std::milli> duration;
}  // namespace

const auto kProgramStartTime = hr_clock::now();

uint32_t time_millis() {
  duration elapsed = hr_clock::now() - kProgramStartTime;
  return static_cast<uint32_t>(elapsed.count());
}