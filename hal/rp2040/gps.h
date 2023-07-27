#pragma once

#include <Arduino.h>

#include "TinyGPS++.h"
#include "board.h"
#include "coordinates.h"

class Gps {
 public:
  void Start();
  void Update(uint32_t millis);
  GpsInfo gps_info() const { return gps_info_; }

 private:
  TinyGPSPlus tiny_gps_;
  UART uart_{/*tx=*/kGpsUartTxPin, /*rx=*/kGpsUartRxPin};
  GpsInfo gps_info_ = {};
};