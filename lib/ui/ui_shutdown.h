#pragma once

#include "coordinates.h"
#include "lvgl.h"
#include "ui_base.h"

// Shutdown screen warns the user when the device is about to shutdown.
class UiShutdown : public UiBase {
 public:
  UiShutdown();
  void update(uint32_t millis);

 private:
  lv_obj_t* diagnostics_text_;
};