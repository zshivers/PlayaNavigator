#include "backlight.h"

#include <cmath>
#include <initializer_list>
#include <limits>

#include "board.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// PWM frequency = 1 KHz.
constexpr int kPwmWrapCount = kSystemClockFreqHz / 1000;
static_assert(kPwmWrapCount < std::numeric_limits<uint16_t>::max());

namespace {
uint16_t gamma(float in) {
  constexpr float gamma = 2.8;
  return (uint16_t)(pow(in, gamma) * static_cast<float>(kPwmWrapCount) + 0.5f);
}
}  // namespace

Backlight::Backlight(int led_w_pin, int led_r_pin, int led_g_pin, int led_b_pin)
    : led_w_pin_(led_w_pin),
      led_r_pin_(led_r_pin),
      led_g_pin_(led_g_pin),
      led_b_pin_(led_b_pin) {}

void Backlight::Start() {
  // Configure all pins for PWM.
  for (int pin : {led_w_pin_, led_r_pin_, led_g_pin_, led_b_pin_}) {
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_wrap(&cfg, kPwmWrapCount);
    pwm_init(pwm_gpio_to_slice_num(pin), &cfg, true);
    gpio_set_function(pin, GPIO_FUNC_PWM);
  }
}

void Backlight::SetBrightness(float brightness) {
  if (brightness < 0.0 || brightness > 1.0) return;
  brightness_ = brightness;
  UpdatePwm();
}

void Backlight::SetColor(float w, float r, float g, float b) {
  w_ = w;
  r_ = r;
  g_ = g;
  b_ = b;
  UpdatePwm();
}

void Backlight::UpdatePwm() {
  pwm_set_gpio_level(led_w_pin_, gamma(brightness_ * w_));
  pwm_set_gpio_level(led_r_pin_, gamma(brightness_ * r_));
  pwm_set_gpio_level(led_g_pin_, gamma(brightness_ * g_));
  pwm_set_gpio_level(led_b_pin_, gamma(brightness_ * b_));
}