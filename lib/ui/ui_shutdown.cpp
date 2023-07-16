#include "ui_shutdown.h"

#include <stdio.h>

#include "coordinates.h"
#include "lvgl.h"

namespace {
constexpr uint32_t kGpsStaleTimeout_ms = 2'500;
}

UiShutdown::UiShutdown() : UiBase() {
  SetTitle("SHUTDOWN");

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

void UiShutdown::update(uint32_t millis) {
//   constexpr char format[] = R"()";
//   char text[100] = "";
//   snprintf(text, sizeof(text), format, GpsStatusString(millis, gps_info),
//            gps_info.hour, gps_info.minute, gps_info.second,
//            gps_info.location.lat, gps_info.location.lon,
//            static_cast<int>(gps_info.satellites));
//   lv_label_set_text(diagnostics_text_, text);
  lv_label_set_text(diagnostics_text_, "Shutdown in 5 secs");
}