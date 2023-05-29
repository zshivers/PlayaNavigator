#pragma once
#include <string>

#include "lvgl.h"

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