#include "ui_location.h"

#include "coordinates.h"
#include "lvgl.h"
#include "playa_config.h"
#include "power.h"

LV_IMG_DECLARE(SpinningMan00);
LV_IMG_DECLARE(SpinningMan01);
LV_IMG_DECLARE(SpinningMan02);
LV_IMG_DECLARE(SpinningMan03);
LV_IMG_DECLARE(SpinningMan04);
LV_IMG_DECLARE(SpinningMan05);
LV_IMG_DECLARE(SpinningMan06);
LV_IMG_DECLARE(SpinningMan07);
LV_IMG_DECLARE(SpinningMan08);
LV_IMG_DECLARE(SpinningMan09);
LV_IMG_DECLARE(SpinningMan10);
LV_IMG_DECLARE(SpinningMan11);
LV_IMG_DECLARE(SpinningMan12);
LV_IMG_DECLARE(SpinningMan13);
LV_IMG_DECLARE(SpinningMan14);
LV_IMG_DECLARE(SpinningMan15);
LV_IMG_DECLARE(SpinningMan16);
LV_IMG_DECLARE(SpinningMan17);
LV_IMG_DECLARE(SpinningMan18);
LV_IMG_DECLARE(SpinningMan19);

namespace {
const lv_img_dsc_t* kSpinningManAnimationImages[20] = {
    &SpinningMan00, &SpinningMan01, &SpinningMan02, &SpinningMan03,
    &SpinningMan04, &SpinningMan05, &SpinningMan06, &SpinningMan07,
    &SpinningMan08, &SpinningMan09, &SpinningMan10, &SpinningMan11,
    &SpinningMan12, &SpinningMan13, &SpinningMan14, &SpinningMan15,
    &SpinningMan16, &SpinningMan17, &SpinningMan18, &SpinningMan19,
};
}

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
  lv_style_set_flex_track_place(&container_style, LV_FLEX_ALIGN_CENTER);
  lv_style_set_layout(&container_style, LV_LAYOUT_FLEX);
  lv_style_set_pad_all(&container_style, 0);
  lv_style_set_pad_column(&container_style, 0);
  lv_style_set_pad_row(&container_style, 0);
  lv_obj_add_style(content_, &container_style, 0);

  lv_obj_add_style(location_text_, &location_text_style, 0);

  animimg_ = lv_animimg_create(content_);
  lv_obj_center(animimg_);
  // lv_animimg_set_src(animimg_, (lv_img_dsc_t**)kSpinningManAnimationImages, 20);
  lv_animimg_set_src(animimg_, (const void**)kSpinningManAnimationImages, 20);
  lv_animimg_set_duration(animimg_, 2000);
  lv_animimg_set_repeat_count(animimg_, LV_ANIM_REPEAT_INFINITE);
  lv_animimg_start(animimg_);

  lv_obj_set_align(animimg_, LV_ALIGN_CENTER);

  EnableBmLogoAnimation(false);
}

void UiLocation::EnableBmLogoAnimation(bool enable) {
  if (enable) {
    lv_obj_add_flag(location_text_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(animimg_, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_clear_flag(location_text_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(animimg_, LV_OBJ_FLAG_HIDDEN);
  }
}

void UiLocation::update(GpsInfo gps_info) {
  if (!gps_info.valid) {
    lv_label_set_text(location_text_, "No GPS");
    EnableBmLogoAnimation(false);
    return;
  }

  const PlayaAddress address =
      LatLonToAddress(kPlayaMapConfig, gps_info.location);

  // If inside the promenade for the man, show the rotating man animation!
  const bool at_the_man = address.radius_m < feetToMeters(400.0);
  EnableBmLogoAnimation(at_the_man);
  if (at_the_man) {
    return;
  }

  char text[50];
  if (address.road.valid) {
    // If on a road, show radial and the road letter.
    snprintf(text, sizeof(text), "%02d:%02d\n%c", address.hour, address.minute,
             address.road.value);
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