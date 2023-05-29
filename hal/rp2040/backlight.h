#pragma once

#include <cstdint>

class Backlight {
 public:
  Backlight(int led_white_pin, int led_r_pin, int led_g_pin, int led_b_pin);
  void SetBrightness(uint8_t brightness);
  void SetColor(uint8_t w, uint8_t r, uint8_t g, uint8_t b);

 private:
  int led_w_pin_;
  int led_r_pin_;
  int led_g_pin_;
  int led_b_pin_;
  float brightness_ = 0;
  float w_ = 0.0, r_ = 0.0, g_ = 0.0, b_ = 0.0;

  void UpdatePwm();
};
