#include "auto_shutdown.h"

#include "hal_time.h"
#include "power.h"

constexpr double kMinSpeed_mph = 0.25;
constexpr uint32_t kShutdownTimeout_ms = 1000 * 60 * 5;     // 5 mins.
constexpr uint32_t kInteractionTimeout_ms = 1000 * 60 * 1;  // 1 min.
constexpr float kMinBatteryVoltage = 3.20f;
constexpr uint32_t kBatteryLowTimeout_ms = 10 * 1000;  // 10 sec.

AutoShutdown::AutoShutdown(Power& power)
    : power_(power), deadline_ms_(kShutdownTimeout_ms) {}

void AutoShutdown::Update(uint32_t millis, uint32_t last_interaction_time_ms,
                          const GpsInfo& gps_info) {
  // Extend the deadline when moving or if interacting.
  const bool moving = gps_info.speed_mph > kMinSpeed_mph;
  const bool interacting = last_interaction_time_ms > kInteractionTimeout_ms;
  if (moving || interacting) {
    deadline_ms_ = time_millis() + kShutdownTimeout_ms;
  }

  // Shutdown if the battery is low.
  const bool battery_good = power_.battery_voltage() >= kMinBatteryVoltage;
  if (battery_good) {
    battery_low_deadline_ms_ = millis + kBatteryLowTimeout_ms;
  } else if (!battery_good && millis > battery_low_deadline_ms_) {
    power_.power_enable(false);
  }

  // Shutdown the power when the deadline expires.
  if (millis > deadline_ms_) {
    power_.power_enable(false);
  }
}