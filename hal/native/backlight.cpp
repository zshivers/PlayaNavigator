#include "backlight.h"

#include <cmath>
#include <initializer_list>

namespace {
uint16_t gamma(float in) {
  float gamma = 2.8;
  return (uint16_t)(pow(in, gamma) * 65535.0f + 0.5f);
}
}  // namespace

Backlight::Backlight(int led_w_pin, int led_r_pin, int led_g_pin, int led_b_pin)
    : led_w_pin_(led_w_pin),
      led_r_pin_(led_r_pin),
      led_g_pin_(led_g_pin),
      led_b_pin_(led_b_pin) {
}

void Backlight::SetBrightness(uint8_t brightness) {
  brightness_ = (float)brightness / 255.0f;
}

void Backlight::SetColor(uint8_t w, uint8_t r, uint8_t g, uint8_t b) {
  w_ = static_cast<float>(w) / 255.0f;
  r_ = static_cast<float>(r) / 255.0f;
  g_ = static_cast<float>(g) / 255.0f;
  b_ = static_cast<float>(b) / 255.0f;
}
