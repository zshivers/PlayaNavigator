#pragma once

#include <cstdint>

#include "coordinates.h"
#include "power.h"

// Automatically shut down the device when:
// - There has been no GPS movement.
// - There has been no interactions with buttons.
// - Battery is low.
//
// 5 seconds prior to shutdown, `reason()` will return the
// cause of the shutdown.
class AutoShutdown {
 public:
  AutoShutdown(Power& power);
  void Update(uint32_t millis, uint32_t last_interaction_time,
              const GpsInfo& gps_info);
  enum class ShutdownReason { kNoShutdown, kBatteryLow, kInactivity };
  ShutdownReason reason() const { return reason_; }

 private:
  Power& power_;
  int32_t active_deadline_ms_ = 0;
  int32_t battery_low_deadline_ms_ = 0;
  ShutdownReason reason_ = ShutdownReason::kNoShutdown;
};