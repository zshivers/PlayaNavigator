#include "backlight.h"

#include <cmath>
#include <initializer_list>

#include "hardware/gpio.h"
#include "hardware/pwm.h"

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
  // Configure all pins for PWM.
  for (int pin : {led_w_pin_, led_r_pin_, led_g_pin_, led_b_pin_}) {
    pwm_config cfg = pwm_get_default_config();
    pwm_set_wrap(pwm_gpio_to_slice_num(pin), 65535);
    pwm_init(pwm_gpio_to_slice_num(pin), &cfg, true);
    gpio_set_function(pin, GPIO_FUNC_PWM);
  }
}

void Backlight::SetBrightness(uint8_t brightness) {
  brightness_ = (float)brightness / 255.0f;
  UpdatePwm();
}

void Backlight::SetColor(uint8_t w, uint8_t r, uint8_t g, uint8_t b) {
  w_ = (float)w / 255.0f;
  r_ = (float)r / 255.0f;
  g_ = (float)g / 255.0f;
  b_ = (float)b / 255.0f;
  UpdatePwm();
}

void Backlight::UpdatePwm() {
  pwm_set_gpio_level(led_w_pin_, gamma(brightness_ * w_));
  pwm_set_gpio_level(led_r_pin_, gamma(brightness_ * r_));
  pwm_set_gpio_level(led_g_pin_, gamma(brightness_ * g_));
  pwm_set_gpio_level(led_b_pin_, gamma(brightness_ * b_));
}