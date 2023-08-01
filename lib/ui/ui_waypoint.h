#pragma once
#include <cinttypes>

#include "backlight.h"
#include "coordinates.h"
#include "lvgl.h"
#include "ui_base.h"
#include "waypoint_storage.h"
#include "periodic.h"

// Waypoint screen is used to navigate from the current location to a fixed
// stored location. It shows the distance and course to the waypoint. Distance
// is shown in text, and direction is shown with a graphical arrow.
//
// This screen has two modes:
// 1. Waypoints - navigate to any of a small set of settable waypoints.
// 2. Bathroom - navigate to the closest bathroom.
//
// Waypoints are stored in non-volatile storage via `waypoint_storage.h`.
class UiWaypoint : public UiBase {
 public:
  UiWaypoint(Backlight* backlight);

  enum class Mode { kWaypoints, kNearestBathroom };
  void SetMode(Mode mode) { mode_ = mode; }

  void Update(GpsInfo gps_info);

  // In waypoint mode, change to the next waypoint. If at the last waypoint,
  // cycle back to the first.
  void IncrementWaypoint();

  // Save the current location into the waypoint currently selected.
  void SaveWaypoint();

 private:
  void DrawDirectionArrow(float angle_deg);
  void DrawCircle();
  void UpdateAddressText(MaybeValid<LatLon> waypoint);
  void EstimateDirection(const GpsInfo& gps_info);

  WaypointStorage ws_;
  Backlight* backlight_;
  GpsInfo gps_info_;

  constexpr static uint8_t kMaxTotalWaypoints = 5;
  const std::array<std::string, kMaxTotalWaypoints> waypoint_names_ = {
      "WAYPOINT CAMP", "WAYPOINT 1", "WAYPOINT 2", "WAYPOINT 3", "WAYPOINT 4"};

  constexpr static int kCanvasWidth = 40;
  constexpr static int kCanvasHeight = 40;
  lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(kCanvasWidth, kCanvasHeight)];

  Mode mode_ = Mode::kWaypoints;
  uint8_t current_waypoint_index_ = 0;

  MaybeValid<LatLon> dir_initial;
  MaybeValid<LatLon> dir_end;
  MaybeValid<double> course_deg_;

  lv_obj_t* distance_label_;
  lv_obj_t* arrow_canvas_;
  lv_obj_t* address_label_;
};