#pragma once

#include "auto_shutdown.h"
#include "backlight.h"
#include "coordinates.h"
#include "lvgl.h"
#include "power.h"
#include "ui_diagnostics.h"
#include "ui_location.h"
#include "ui_shutdown.h"
#include "ui_splash.h"
#include "ui_waypoint.h"

class Ui {
 public:
  Ui(const PlayaMapConfig& map_config, const std::vector<LatLon>& bathrooms,
     Power& power, Backlight& backlight);
  enum Mode {
    kSplash,
    kLocation,
    kWaypoint,
    kBathroom,
    kDiagnostics,
    kShutdown
  };
  void SetMode(Mode mode);
  void update(uint32_t millis);

  enum class Button { kA, kB, kC, kD, kE };
  void ButtonPress(Button button);
  void ButtonLongPress(Button button);

  void OnGpsInfo(const GpsInfo& gps_info) { gps_info_ = gps_info; }

 private:
  Mode mode_;
  Power& power_;
  Backlight& backlight_;

  // Can switch to any of these different UI pages.
  UiSplash ui_splash_;
  UiLocation ui_location_;
  UiWaypoint ui_waypoint_;
  UiDiagnostics ui_diagnostics_;
  UiShutdown ui_shutdown_;

  GpsInfo gps_info_;
  AutoShutdown auto_shutdown_;
  bool first_update_ = true;
  uint32_t last_button_press_ms_ = 0;
  unsigned int brightness_index_ = 0;
  AutoShutdown::ShutdownReason shutdown_reason_ =
      AutoShutdown::ShutdownReason::kNoShutdown;
};