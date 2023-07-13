#pragma once
#include <string>

#include "lvgl.h"

// Common setup for all UI screens. Creates a window with a header, inverted background
// and title text.
class UiBase {
 public:
  UiBase();
  lv_obj_t* screen() { return screen_; }

 protected:
  void SetTitle(std::string title);
  lv_obj_t* screen_;
  lv_obj_t* title_;
  lv_obj_t* window_;
  lv_obj_t* content_;
};