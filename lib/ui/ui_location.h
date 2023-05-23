#pragma once

#include "lvgl.h"

class UiLocation {
public:
    UiLocation(lv_obj_t * parent);
    void update();

private:
   lv_obj_t * radial_label_;
   lv_obj_t * distance_label_;
};