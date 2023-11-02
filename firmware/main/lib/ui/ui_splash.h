#pragma once
#include <cstdint>

#include "lvgl.h"

// Show a full-screen splash animation.
class UiSplash {
 public:
  UiSplash();
  lv_obj_t* screen() { return screen_; }
  void Start();
  bool Complete();

 protected:
  lv_obj_t* screen_;
  lv_obj_t* animimg_;
};