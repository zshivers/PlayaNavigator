#include "ui_location.h"

#include "coordinates.h"
#include "lvgl.h"
#include "playa_config.h"

UiLocation::UiLocation() : UiBase() {
  SetTitle("LOCATION");

  location_text_ = lv_label_create(content_);
  lv_label_set_text(location_text_, "");

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_radius(&style, 0);
  lv_style_set_border_color(&style, lv_color_black());
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  lv_style_set_width(&style, LV_PCT(100));
  lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

  static lv_style_t container_style;
  lv_style_init(&container_style);
  lv_style_set_pad_column(&container_style, 0);
  lv_obj_add_style(content_, &container_style, 0);
  lv_style_set_flex_flow(&container_style, LV_FLEX_FLOW_COLUMN);
  lv_style_set_flex_main_place(&container_style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_flex_cross_place(&container_style, LV_FLEX_ALIGN_CENTER);
  lv_style_set_layout(&container_style, LV_LAYOUT_FLEX);
  lv_style_set_pad_all(&container_style, 0);
  lv_style_set_pad_column(&container_style, 0);
  lv_style_set_pad_row(&container_style, 0);
  lv_obj_add_style(content_, &container_style, 0);

  lv_obj_add_style(location_text_, &style, 0);
}

void UiLocation::update(GpsInfo gps_info) {
  if (!gps_info.valid) {
    lv_label_set_text(location_text_, "No GPS");
    return;
  }

  const MaybeValid<PlayaAddress> address = LatLonToAddress(
      kPlayaMapConfig, {.lat = gps_info.lat, .lon = gps_info.lon});
  char text[50];
  if (!address.valid) {
    lv_label_set_text(location_text_, "Invalid");
  }

  if (address.value.road.valid) {
    snprintf(text, 50, "%02d:%02d\n%c", address.value.hour,
             address.value.minute, address.value.road.value);
  } else {
    const double feetToMan = feetToMeters(address.value.radius_m);
    if (feetToMan < 9999.0) {
      snprintf(text, 50, "%02d:%02d\n%d ft", address.value.hour,
               address.value.minute, (int)feetToMan);
    } else {
      const double milesToMan = metersToMiles(address.value.radius_m);
      snprintf(text, 50, "%02d:%02d\n%.1f mi", address.value.hour,
               address.value.minute, milesToMan);
    }
  }
  lv_label_set_text(location_text_, text);
}