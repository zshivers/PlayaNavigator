#pragma once
#include <cinttypes>

#include "backlight.h"
#include "coordinates.h"
#include "lvgl.h"
#include "ui_base.h"
#include "waypoint_storage.h"

class UiWaypoint : public UiBase {
 public:
  UiWaypoint(Backlight* backlight);
  void Update(GpsInfo gps_info);
  void IncrementWaypoint();
  void SaveWaypoint();

  enum class Mode { kWaypoints, kNearestBathroom };
  void SetMode(Mode mode) { mode_ = mode; }

 private:
  void draw_arrow(float angle_deg);
  void draw_circle();
  uint8_t current_waypoint_index_ = 0;
  lv_obj_t* distance_label_;
  lv_obj_t* canvas_;
  WaypointStorage ws_;
  Backlight* backlight_;
  GpsInfo gps_info_;
  Mode mode_ = Mode::kWaypoints;
};