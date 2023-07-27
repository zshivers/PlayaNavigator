#pragma once

#include "coordinates.h"
#include "lvgl.h"
#include "power.h"
#include "ui_base.h"

// Diagnostics screen shows useful debugging details
// GPS diagnostics: fix quality, time, location, number of satellites.
// Board diagnostics: Battery voltage, charging state, USB plugged state.
class UiDiagnostics : public UiBase {
 public:
  UiDiagnostics(Power& power);
  void update(uint32_t millis, GpsInfo gps_info);
  void GoToNextPage() {
    if (page_ == kGps) page_ = kBattery;
    else if (page_ == kBattery) page_ = kGps;
  }

 private:
  Power& power_;
  enum Page { kGps, kBattery } page_ = kGps;
  lv_obj_t* diagnostics_text_;
  bool last_pps_state_ = false;
  uint8_t pps_count_ = 0;
  uint32_t last_uart_time_ = 0;
  uint8_t uart_message_count_ = 0;
};