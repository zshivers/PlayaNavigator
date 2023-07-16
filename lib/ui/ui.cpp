#include "ui.h"

#include <iostream>
#include <memory>

#include "coordinates.h"
#include "hal_time.h"
#include "lvgl.h"
#include "power.h"
#include "ui_location.h"
#include "ui_waypoint.h"

// If the device is going to shutdown within this duration, switch to the shutdown UI.
constexpr uint32_t kShutdownWarningTimeout_ms = 30 * 1000; // 30 sec.

Ui::Ui(Power& power, Backlight* backlight)
    : power_(power), backlight_(backlight), auto_shutdown_(power) {
  ui_location_ = std::make_unique<UiLocation>();
  ui_waypoint_ = std::make_unique<UiWaypoint>(backlight);
  ui_diagnostics_ = std::make_unique<UiDiagnostics>(power_);
  ui_shutdown_ = std::make_unique<UiShutdown>();
  SetMode(kLocation);
}

void Ui::update(uint32_t millis) {
  auto_shutdown_.Update(millis, last_button_press_ms_, gps_info_);
  if (millis + kShutdownWarningTimeout_ms > auto_shutdown_.deadline()) {
    mode_ = Mode::kShutdown;
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
    case Mode::kShutdown:
      ui_shutdown_->update(millis);
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
    case Button::kD: {
      constexpr std::array<uint8_t, 4> kBrightnessSteps = {0, 80, 120, 200};
      brightness_index_++;
      if (brightness_index_ >= kBrightnessSteps.size()) brightness_index_ = 0;
      backlight_->SetBrightness(kBrightnessSteps[brightness_index_]);
      break;
    }
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
    case Button::kE:
      // Turn off the power to the battery.
      power_.power_enable(false);
      break;
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
