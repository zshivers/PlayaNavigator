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
    if (page_ == kGps1) page_ = kGps2;
    else if (page_ == kGps2) page_ = kBattery;
    else if (page_ == kBattery) page_ = kMap;
    else if (page_ == kMap) page_ = kGps1;
  }

 private:
  Power& power_;
  enum Page { kGps1, kGps2, kBattery, kMap } page_ = kGps1;
  lv_obj_t* diagnostics_text_;
  bool last_pps_state_ = false;
  uint8_t pps_count_ = 0;
  uint32_t last_uart_time_ = 0;
  uint8_t uart_message_count_ = 0;
};