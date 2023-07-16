#pragma once

#include "coordinates.h"
#include "power.h"

class AutoShutdown {
 public:
  AutoShutdown(Power& power);
  void Update(uint32_t millis, uint32_t last_interaction_time, const GpsInfo& gps_info);
  uint32_t deadline() const { return deadline_ms_; }

 private:
  Power& power_;
  uint32_t deadline_ms_ = 0;
  uint32_t battery_low_deadline_ms_ = 0;
};