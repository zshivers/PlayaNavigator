#pragma once

#include "lvgl.h"
#include "ui_location.h"
#include "ui_waypoint.h"

class Ui {
 public:
  Ui();
  enum Mode { kLocation, kWaypoint, kDiagnostics };
  void SetMode(Mode mode);

 private:
  lv_obj_t* screens_[2];
  lv_obj_t* screen_labels_[2];
  UiLocation* ui_location_;
  UiWaypoint* ui_waypoint_;
};