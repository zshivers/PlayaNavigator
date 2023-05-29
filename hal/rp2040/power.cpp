#include "power.h"

#include "hardware/gpio.h"

constexpr int k3V3EnablePin = 10;

void power_enable(bool on) {
  gpio_set_function(k3V3EnablePin, GPIO_FUNC_SIO);
  gpio_set_dir(k3V3EnablePin, GPIO_OUT);
  gpio_put(k3V3EnablePin, on);
}