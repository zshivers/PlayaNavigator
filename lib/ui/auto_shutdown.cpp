#include "auto_shutdown.h"

#include "hal_time.h"

constexpr double kMinSpeed_mph = 0.25;
constexpr uint32_t kShutdownTimeoutMs = 1000 * 60 * 5;   // 5 mins.
constexpr uint32_t kInteractionTimeout = 1000 * 60 * 1;  // 1 min.

AutoShutdown::AutoShutdown() : deadline_ms_(kShutdownTimeoutMs) {}

uint32_t AutoShutdown::update(uint32_t last_interaction_time,
                              const GpsInfo& gps_info) {
  const bool moving = gps_info.speed_mph > kMinSpeed_mph;
  const bool interacting = last_interaction_time > kInteractionTimeout;
  if (moving || interacting) {
    deadline_ms_ = time_millis() + kShutdownTimeoutMs;
  }
  return deadline_ms_;
}