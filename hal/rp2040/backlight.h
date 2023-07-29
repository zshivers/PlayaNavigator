#pragma once

#include <cstdint>

class Backlight {
 public:
  Backlight(int led_white_pin, int led_r_pin, int led_g_pin, int led_b_pin);
  
  // Brightness in range [0.0, 1.0].
  void SetBrightness(float brightness);

  // Colors in range [0.0, 1.0].
  void SetColor(float w, float r, float g, float b);

 private:
  void UpdatePwm();

  const int led_w_pin_;
  const int led_r_pin_;
  const int led_g_pin_;
  const int led_b_pin_;
  float brightness_ = 0;
  float w_ = 0.0, r_ = 0.0, g_ = 0.0, b_ = 0.0;
};
