#pragma once

#include "TinyGPS++.h"
#include "coordinates.h"

class Gps {
 public:
  Gps();
  void update();
  GpsInfo gps_info() const { return gps_info_; }

 private:
  TinyGPSPlus tiny_gps_;
  UART uart_;
  GpsInfo gps_info_;
};