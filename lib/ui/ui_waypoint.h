#pragma once

#include "lvgl.h"

class UiWaypoint {
public:
    UiWaypoint(lv_obj_t* parent);
    void update();
private:
   lv_obj_t * time_label_;
   lv_obj_t * direction_meter_;
};