#pragma once

#include <condition_variable>
#include <mutex>
#include <string>

#include "coordinates.h"

class Gps {
 public:
  void Start();
  void Update(uint32_t millis) {}
  GpsInfo gps_info();

 private:
  GpsInfo gps_info_ = {.valid = false, .satellites = 4, .speed_mph = 0.0};
  std::mutex m;
  bool error = false;
};