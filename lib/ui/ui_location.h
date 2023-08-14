#pragma once

#include "coordinates.h"
#include "lvgl.h"
#include "ui_base.h"

// Location screen shows the current location in a large font, for easy reading
// while biking or walking. The location is shown in the most helpful
// address format that makes sense for the current location.
//
// Example location formats:
// - 10:10 / A
// - 12:10 / 1000 ft
// - 03:00 / 1.0 mi
//
// If GPS is not availble, "No GPS" is shown instead of location.
class UiLocation : public UiBase {
 public:
  UiLocation();
  void update(GpsInfo gps_info);

 private:
  void EnableBmLogoAnimation(bool enable);
  lv_obj_t* location_text_;
  lv_obj_t* animimg_;
};