#pragma once

#include "backlight.h"
#include "coordinates.h"
#include "lvgl.h"
#include "ui_base.h"

class UiDiagnostics : public UiBase {
 public:
  UiDiagnostics();
  void update(uint32_t millis, GpsInfo gps_info);

 private:
  lv_obj_t* diagnostics_text_;
  Backlight* backlight_;
};