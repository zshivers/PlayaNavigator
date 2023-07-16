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
};