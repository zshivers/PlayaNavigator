#include "ui_diagnostics.h"

#include <stdio.h>

#include "coordinates.h"
#include "lvgl.h"

namespace {
constexpr uint32_t kGpsStaleTimeout_ms = 2'500;

const char* GpsStatusString(uint32_t millis, const GpsInfo& gps_info) {
  if (millis - gps_info.update_time > kGpsStaleTimeout_ms) {
    return "Disconnect";
  }
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
}  // namespace

UiDiagnostics::UiDiagnostics() : UiBase() {
  SetTitle("DIAGNOSTICS");

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_radius(&style, 0);
  lv_style_set_border_color(&style, lv_color_black());
  lv_style_set_text_font(&style, &lv_font_unscii_8);

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
  lv_obj_add_style(diagnostics_text_, &style, 0);
}

void UiDiagnostics::update(uint32_t millis, GpsInfo gps_info) {
  const char fmt[] = R"( GPS:%s
Time:%02d:%02d:%02d
 Lat:%.6f
 Lon:%.6f
Sats:%d)";
  char s[100] = "";
  snprintf(s, 100, fmt, GpsStatusString(millis, gps_info), gps_info.hour,
           gps_info.minute, gps_info.second, gps_info.lat, gps_info.lon,
           gps_info.satellites);
  lv_label_set_text(diagnostics_text_, s);
}