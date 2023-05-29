#pragma once

#include "lvgl.h"
#include "ui_base.h"
#include "coordinates.h"

class UiLocation : public UiBase {
 public:
  UiLocation();
  void update(GpsInfo gps_info);

 private:
  lv_obj_t* location_text_;
};