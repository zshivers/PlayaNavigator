#pragma once

#include <memory>

#include "auto_shutdown.h"
#include "backlight.h"
#include "power.h"
#include "coordinates.h"
#include "lvgl.h"
#include "ui_diagnostics.h"
#include "ui_location.h"
#include "ui_shutdown.h"
#include "ui_waypoint.h"

class Ui {
 public:
  Ui(Power& power, Backlight* backlight);
  enum Mode { kLocation, kWaypoint, kBathroom, kDiagnostics, kShutdown };
  void SetMode(Mode mode);
  void update(uint32_t millis);

  enum class Button { kA, kB, kC, kD, kE };
  void ButtonPress(Button button);
  void ButtonLongPress(Button button);

  void OnGpsInfo(const GpsInfo& gps_info) { gps_info_ = gps_info; }

 private:
  Mode mode_;
  std::unique_ptr<UiLocation> ui_location_;
  std::unique_ptr<UiWaypoint> ui_waypoint_;
  std::unique_ptr<UiDiagnostics> ui_diagnostics_;
  std::unique_ptr<UiShutdown> ui_shutdown_;
  Power& power_;
  Backlight* backlight_;
  GpsInfo gps_info_;
  AutoShutdown auto_shutdown_;
  uint32_t last_button_press_ms_ = 0;
  unsigned int brightness_index_ = 0;
  AutoShutdown::ShutdownReason shutdown_reason_ = AutoShutdown::ShutdownReason::kNoShutdown;
};