#include "auto_shutdown.h"

#include "hal_time.h"
#include "power.h"

constexpr float kMinBatteryVoltage = 3.20f;
constexpr double kMinSpeed_mph = 0.25;
// constexpr uint32_t kActiveTimeout_ms = 1000 * 60 * 5;       // 5 mins.
// constexpr uint32_t kInteractionTimeout_ms = 1000 * 60 * 1;  // 1 min.
constexpr uint32_t kActiveTimeout_ms = 1000 * 10;
constexpr uint32_t kInteractionTimeout_ms = 1000 * 10;
constexpr uint32_t kBatteryLowTimeout_ms = 10 * 1000;       // 10 sec.

AutoShutdown::AutoShutdown(Power& power)
    : power_(power),
      active_deadline_ms_(kActiveTimeout_ms),
      battery_low_deadline_ms_(kBatteryLowTimeout_ms) {}

void AutoShutdown::Update(uint32_t millis, uint32_t last_interaction_time_ms,
                          const GpsInfo& gps_info) {
  // Determine if the device is being actively used. Activity is defined as
  // moving with sufficient speed or interacting with the buttons.
  const bool moving = gps_info.valid && gps_info.speed_mph > kMinSpeed_mph;
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

  if (battery_low_deadline_ms_ - (int32_t)millis < 5'000) {
    reason_ = ShutdownReason::kBatteryLow;
  } else if (active_deadline_ms_ - (int32_t)millis < 5'000) {
    reason_ = ShutdownReason::kInactivity;
  } else {
    reason_ = ShutdownReason::kNoShutdown;
  }

  const uint32_t deadline_ms_ =
      std::min(active_deadline_ms_, battery_low_deadline_ms_);

  // Shutdown the power when the deadline expires.
  if (millis > deadline_ms_) {
    power_.power_enable(false);
  }
}