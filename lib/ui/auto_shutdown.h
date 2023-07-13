#pragma once

#include "coordinates.h"

class AutoShutdown {
 public:
  AutoShutdown();
  uint32_t update(uint32_t last_interaction_time, const GpsInfo& gps_info);

 private:
  uint32_t deadline_ms_ = 0;
};