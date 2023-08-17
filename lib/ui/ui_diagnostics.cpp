#include "ui_diagnostics.h"

#include <stdio.h>

#include "coordinates.h"
#include "lvgl.h"
#include "playa_config.h"
#include "power.h"

namespace {
constexpr uint32_t kGpsStaleTimeout_ms = 2'500;

const char* GpsStatusString(uint32_t millis, const GpsInfo& gps_info) {
  if (millis - gps_info.uart_time > kGpsStaleTimeout_ms) {
    return "Disconnect";  // Cannot fit "Disconnected".
  }
  if (millis - gps_info.update_time > kGpsStaleTimeout_ms) {
    return "No Fix";
  }
  // Mapping from HDOP to quality string is from
  // https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)
  if (gps_info.hdop <= 2) {
    return "Excellent";
  } else if (gps_info.hdop <= 5) {
    return "Good";
  } else if (gps_info.hdop <= 10) {
    return "Moderate";
  } else if (gps_info.hdop <= 20) {
    return "Fair";
  } else {
    return "Poor";
  }
}

float inline degCtoF(float deg_c) { return (9.0 / 5.0) * deg_c + 32; }

}  // namespace

UiDiagnostics::UiDiagnostics(Power& power) : UiBase(), power_(power) {
  SetTitle("DIAGNOSTICS");

  static lv_style_t text_style;
  lv_style_init(&text_style);
  lv_style_set_radius(&text_style, 0);
  lv_style_set_border_color(&text_style, lv_color_black());
  lv_style_set_text_font(&text_style, &lv_font_unscii_8);

  static lv_style_t container_style;
  lv_style_init(&container_style);
  lv_style_set_pad_column(&container_style, 0);
  lv_obj_add_style(content_, &container_style, 0);
  lv_style_set_flex_flow(&container_style, LV_FLEX_FLOW_COLUMN_WRAP);
  lv_style_set_flex_main_place(&container_style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_flex_cross_place(&container_style, LV_FLEX_ALIGN_CENTER);
  lv_style_set_layout(&container_style, LV_LAYOUT_FLEX);
  lv_style_set_pad_all(&container_style, 0);
  lv_style_set_pad_column(&container_style, 0);
  lv_obj_add_style(content_, &container_style, 0);

  diagnostics_text_ = lv_label_create(content_);
  lv_obj_add_style(diagnostics_text_, &text_style, 0);
}

void UiDiagnostics::update(uint32_t millis, GpsInfo gps_info) {
  // Count PPS edges.
  const bool pps_state = power_.gps_pps();
  if (pps_state && !last_pps_state_) {
    pps_count_++;
  }
  last_pps_state_ = pps_state;

  if (gps_info.uart_time != last_uart_time_) {
    uart_message_count_++;
  }
  last_uart_time_ = gps_info.uart_time;

  char text[110] = "";
  switch (page_) {
    case kGps1: {
      constexpr char format[] = R"( GPS:%s
Time:%02d:%02d:%02d
 Lat:%.6f
 Lon:%.6f
Brng:%+4d deg
 Spd:%.1f mph)";
      snprintf(text, sizeof(text), format, GpsStatusString(millis, gps_info),
               gps_info.hour, gps_info.minute, gps_info.second,
               gps_info.location.lat, gps_info.location.lon,
               static_cast<int>(gps_info.course_deg), gps_info.speed_mph);
      break;
    }

    case kGps2: {
      constexpr char format[] = R"(   HDOP:%.1f
   Sats:%-3d
   UART:%-3d
    PPS:%c%d)";
      snprintf(text, sizeof(text), format, gps_info.hdop,
               static_cast<int>(gps_info.satellites), uart_message_count_,
               pps_state ? '*' : ' ', pps_count_);

      break;
    }

    case kBattery: {
      constexpr char format[] = R"(Voltage:%.2f V
 Charge:%d
    USB:%d
   Temp:%.1f F)";
      snprintf(text, sizeof(text), format, power_.battery_voltage(),
               power_.battery_charging(), power_.usb_plugged(),
               degCtoF(power_.temperature()));
      break;
    }

    case kMap: {
      constexpr char format[] = R"( Map:%s
Cntr:%.6f
     %.6f
  Bathrms:%zu)";
      snprintf(text, sizeof(text), format, kPlayaMapConfig.name,
               kPlayaMapConfig.center.lat, kPlayaMapConfig.center.lon,
               kBathroomLocations.size());
      break;
    }
  }
  lv_label_set_text(diagnostics_text_, text);
}