#pragma once

#include <cstdint>

class Backlight {
 public:
  Backlight(int led_white_pin, int led_r_pin, int led_g_pin, int led_b_pin);
  void SetBrightness(float brightness);
  void SetColor(uint8_t w, uint8_t r, uint8_t g, uint8_t b);

 private:
  const int led_w_pin_;
  const int led_r_pin_;
  const int led_g_pin_;
  const int led_b_pin_;
  float brightness_ = 0;
  float w_ = 0.0, r_ = 0.0, g_ = 0.0, b_ = 0.0;
};
