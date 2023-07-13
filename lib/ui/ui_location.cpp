#include "ui_location.h"

#include "coordinates.h"
#include "lvgl.h"
#include "playa_config.h"
#include "power.h"

#include <iostream>

UiLocation::UiLocation() : UiBase() {
  SetTitle("LOCATION");

  location_text_ = lv_label_create(content_);
  lv_label_set_text(location_text_, "");

  static lv_style_t location_text_style;
  lv_style_init(&location_text_style);
  lv_style_set_radius(&location_text_style, 0);
  lv_style_set_border_color(&location_text_style, lv_color_black());
  lv_style_set_text_font(&location_text_style, &lv_font_unscii_16);
  lv_style_set_width(&location_text_style, LV_PCT(100));
  lv_style_set_text_align(&location_text_style, LV_TEXT_ALIGN_CENTER);

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

  lv_obj_add_style(location_text_, &location_text_style, 0);
}

void UiLocation::update(GpsInfo gps_info) {
  if (!gps_info.valid) {
    lv_label_set_text(location_text_, "No GPS");
    return;
  }

  const PlayaAddress address = LatLonToAddress(kPlayaMapConfig, gps_info.location);
  char text[50];
  if (address.road.valid) {
    // If on a road, show radial and the road letter.
    snprintf(text, sizeof(text), "%02d:%02d\n%c", address.hour,
             address.minute, address.road.value);
  } else {
    // If not on a road, show radial and distance from man. Switch between feet
    // and miles.
    const double feetToMan = metersToFeet(address.radius_m);
    if (feetToMan <= 9999.0) {
      snprintf(text, sizeof(text), "%02d:%02d\n%d ft", address.hour,
               address.minute, static_cast<int>(feetToMan));
    } else {
      const double milesToMan = metersToMiles(address.radius_m);
      snprintf(text, sizeof(text), "%02d:%02d\n%.1f mi", address.hour,
               address.minute, milesToMan);
    }
  }
  lv_label_set_text(location_text_, text);
}