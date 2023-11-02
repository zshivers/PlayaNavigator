#include "auto_shutdown.h"

#include <cmath>
#include <limits>

#include "hal_time.h"
#include "power.h"

constexpr float kMinBatteryVoltage = 3.20f;
constexpr uint32_t kActiveTimeout_ms = 1000 * 60 * 5;       // 5 mins.
constexpr uint32_t kInteractionTimeout_ms = 1000 * 60 * 1;  // 1 min.
constexpr uint32_t kBatteryLowTimeout_ms = 10 * 1000;       // 10 sec.
constexpr int32_t kShutdownAlertTime = 5 * 1000;            // 5 sec.

// Movement filter uses a 10 second history. This effectively makes the speed
// limit for "not moving" as 5 meters / 10 sec = 0.5 m/s ~= 1.1 mph. This is
// much lower than typical walking speed of 3.4 mph.
constexpr double kMaxGpsLocationSpread_m = 5;

AutoShutdown::AutoShutdown(Power& power)
    : power_(power),
      active_deadline_ms_(kActiveTimeout_ms),
      battery_low_deadline_ms_(kBatteryLowTimeout_ms) {}

void AutoShutdown::Update(uint32_t millis, uint32_t last_interaction_time_ms,
                          const GpsInfo& gps_info) {
  // Determine if the device is being actively used. Activity is defined as
  // moving with sufficient speed or interacting with the buttons.

  bool moving = !gps_info.valid;  // If invalid, assume moving, so there's no
                                  // shutdown when looking for GPS signal.
  if (gps_info.valid && gps_info.update_time != location_update_ms_) {
    location_history_[location_history_next_] = gps_info.location;
    if (++location_history_next_ >= location_history_.size())
      location_history_next_ = 0;
    // Find total X-Y spread in the location history.
    double lat_min = std::numeric_limits<double>::infinity();
    double lon_min = std::numeric_limits<double>::infinity();
    double lat_max = -std::numeric_limits<double>::infinity();
    double lon_max = -std::numeric_limits<double>::infinity();

    for (const LatLon location : location_history_) {
      if (location.lat < lat_min) lat_min = location.lat;
      if (location.lon < lon_min) lon_min = location.lon;
      if (location.lat > lat_max) lat_max = location.lat;
      if (location.lon > lon_max) lon_max = location.lon;
    }

    // This is a very crude approximation, but good∏ tradeoff for low speeds.
    const double x_delta_m = std::abs(111111.0 * (lat_max - lat_min));
    const double y_delta_m = std::abs(111111.0 * (lon_max - lon_min));
    location_update_ms_ = gps_info.update_time;

    moving = !(x_delta_m < kMaxGpsLocationSpread_m &&
               y_delta_m < kMaxGpsLocationSpread_m);
  }

  const bool interacting =
      ((int32_t)millis - last_interaction_time_ms) < kInteractionTimeout_ms;
  if (moving || interacting) {
    active_deadline_ms_ = millis + kActiveTimeout_ms;
  }

  // Determine if the battery is low.
  const bool battery_good = power_.battery_voltage() >= kMinBatteryVoltage;
  if (battery_good) {
    battery_low_deadline_ms_ = millis + kBatteryLowTimeout_ms;
  }

  // The reason for shutdown is determined by priority order of the current
  // timeouts.
  if (battery_low_deadline_ms_ - (int32_t)millis < kShutdownAlertTime) {
    reason_ = ShutdownReason::kBatteryLow;
  } else if (active_deadline_ms_ - (int32_t)millis < kShutdownAlertTime) {
    reason_ = ShutdownReason::kInactivity;
  } else {
    reason_ = ShutdownReason::kNoShutdown;
  }

  // Shutdown the power when any deadline expires.
  const uint32_t deadline_ms_ =
      std::min(active_deadline_ms_, battery_low_deadline_ms_);
  if (millis > deadline_ms_) {
    power_.power_enable(false);
  }
}

// bool DetectMovement(const GpsInfo gps_info) {
//   if (gps_info.valid && gps_info.update_time != location_update_ms_) {
//     location_history_[location_history_next_] = gps_info.location;
//     if (++location_history_next_ >= location_history_.size())
//     location_history_next_ = 0;
//     // Find 2D variance in location.
//     double lat_min = std::numeric_limits<double>::infinity();
//     double lon_min = std::numeric_limits<double>::infinity();
//     double lat_max = -std::numeric_limits<double>::infinity();
//     double lon_max = -std::numeric_limits<double>::infinity();

//     for (const LatLon location : location_history_) {
//       if (location.lat < lat_min) lat_min = location.lat;
//       if (location.lon < lon_min) lon_min = location.lon;
//       if (location.lat > lat_max) lat_max = location.lat;
//       if (location.lon > lon_max) lon_max = location.lon;
//     }

//     const double lat_delta = std::abs(lat_max - lat_min);
//     const double lon_delta = std::abs(lon_max - lon_min);

//     // const moving = !(lat_delta < kMaxGpsLocationSpread_m && lon_delta <
//     kMaxGpsLocationSpread_m);
//   }
// }