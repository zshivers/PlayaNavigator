#include "ui.h"

#include <memory>

#include "coordinates.h"
#include "lvgl.h"
#include "ui_location.h"
#include "ui_waypoint.h"
#include "power.h"
#include "hal_time.h"

#include <iostream>

Ui::Ui(Backlight* backlight, Power* power) : power_(power) {
  ui_location_ = std::make_unique<UiLocation>();
  ui_waypoint_ = std::make_unique<UiWaypoint>(backlight);
  ui_diagnostics_ = std::make_unique<UiDiagnostics>(power_);
  SetMode(kLocation);
}

void Ui::update(uint32_t millis) {
  uint32_t shutdown_time = auto_shutdown_.update(last_button_press_ms_, gps_info_);
  if (shutdown_time - time_millis() <= (1000 * 10)) {
  }

  switch (mode_) {
    case Mode::kLocation: 
      ui_location_->update(gps_info_);
      break;
    case Mode::kWaypoint:
    case Mode::kBathroom:
      ui_waypoint_->Update(gps_info_);
      break;
    case Mode::kDiagnostics:
      ui_diagnostics_->update(millis, gps_info_);
      break;
    default:
      break;
  }
}

void Ui::ButtonPress(Ui::Button button) {
  last_button_press_ms_ = time_millis();
  switch (button) {
    case Button::kA:
      if (mode_ == Mode::kDiagnostics) {
        ui_diagnostics_->GoToNextPage();
      } else {
        SetMode(Mode::kLocation);
      }
      break;
    case Button::kB:
      // If already in the waypoint mode, go to next waypoint.
      if (mode_ == Mode::kWaypoint) {
        ui_waypoint_->IncrementWaypoint();
      } else {
        SetMode(Mode::kWaypoint);
        ui_waypoint_->SetMode(UiWaypoint::Mode::kWaypoints);
      }
      break;
    case Button::kC:
      SetMode(Mode::kBathroom);
      ui_waypoint_->SetMode(UiWaypoint::Mode::kNearestBathroom);
      break;
    default:
      break;
  }
}

void Ui::ButtonLongPress(Ui::Button button) {
  last_button_press_ms_ = time_millis();
  switch (button) {
    case Button::kA:
      if (mode_ == Mode::kLocation) {
        SetMode(Mode::kDiagnostics);
      } else {
        SetMode(Mode::kLocation);
      }
      break;
    case Button::kB:
      ui_waypoint_->SaveWaypoint();
      break;
    // case Button::kE:
    //   // Turn off the power to the battery.
    //   power_->power_enable(false);
    //   break;
    default:
      break;
  }
}

void Ui::SetMode(Mode mode) {
  switch (mode) {
    case Mode::kLocation:
      lv_scr_load(ui_location_->screen());
      break;
    case Mode::kWaypoint:
    case Mode::kBathroom:
      lv_scr_load(ui_waypoint_->screen());
      break;
    case Mode::kDiagnostics:
      lv_scr_load(ui_diagnostics_->screen());
      break;
    default:
      break;
  }
  mode_ = mode;
}
