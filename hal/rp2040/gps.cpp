#include "gps.h"

#include "TinyGPS++.h"
#include "board.h"
#include "coordinates.h"

namespace {
constexpr int8_t kTimeZoneOffsetFromUtc = -7;  // Pacific time
uint8_t HourAdjustWithTimezone(uint8_t utc_hour) {
  int8_t hour = utc_hour + kTimeZoneOffsetFromUtc;
  if (hour < 0)
    hour += 24;
  else if (hour > 24)
    hour -= 24;
  return hour;
}
}  // namespace

void Gps::Start() { uart_.begin(9600); }

void Gps::Update(uint32_t millis) {
  int bytes_rx = 0;
  while (uart_.available() > 0) {
    char r = uart_.read();
    tiny_gps_.encode(r);
    // Serial.print(r);
    bytes_rx++;
  }
  if (bytes_rx > 0) gps_info_.uart_time = millis;

  if (!tiny_gps_.location.isValid()) {
    return;
  }

  gps_info_.valid = true;
  if (tiny_gps_.location.isUpdated()) gps_info_.update_time = millis;
  gps_info_.location.lat = tiny_gps_.location.lat();
  gps_info_.location.lon = tiny_gps_.location.lng();

  if (tiny_gps_.time.isValid()) {
    auto& t = tiny_gps_.time;
    gps_info_.hour = HourAdjustWithTimezone(t.hour());
    gps_info_.minute = t.minute();
    gps_info_.second = t.second();
  }
  if (tiny_gps_.hdop.isValid()) {
    gps_info_.hdop = tiny_gps_.hdop.hdop();
  }
  if (tiny_gps_.satellites.isValid()) {
    gps_info_.satellites = tiny_gps_.satellites.value();
  }
  if (tiny_gps_.course.isValid()) {
    gps_info_.course_deg = tiny_gps_.course.deg();
  }
  if (tiny_gps_.speed.isValid()) {
    gps_info_.speed_mph = tiny_gps_.speed.mph();
  }
}
