#include "power.h"

#include "board.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/time.h"

void Power::Start() {
  // ADC setup.
  adc_init();
  adc_gpio_init(kVSysAdcInputPin);  // Connected to resistor divider on VSYS.

  // Power enable.
  gpio_set_function(kPowerEnablePin, GPIO_FUNC_SIO);
  gpio_set_dir(kPowerEnablePin, GPIO_OUT);

  // GPS enable.
  gpio_set_function(kGpsEnablePin, GPIO_FUNC_SIO);
  gpio_set_dir(kGpsEnablePin, GPIO_OUT);

  // GPS PPS input.
  // TODO put this in a better place.
  gpio_set_function(kGpsPpsPin, GPIO_FUNC_SIO);
  gpio_set_dir(kGpsPpsPin, GPIO_IN);

  // Battery charging input.
  gpio_set_function(kChargingPin, GPIO_FUNC_SIO);
  gpio_set_pulls(kChargingPin, true, false);
  gpio_set_dir(kChargingPin, GPIO_IN);

  // // USB plugged input.
  gpio_set_function(kUsbPluggedPin, GPIO_FUNC_SIO);
  gpio_set_dir(kUsbPluggedPin, GPIO_IN);
}

void Power::power_enable(bool on) {
  gpio_put(kPowerEnablePin, on);
  if (!on) sleep_ms(2000);
}

void Power::gps_enable(bool on) {
  gpio_put(kGpsEnablePin, !on);  // Low = power on, High = power off.
}

bool Power::gps_pps() const { return gpio_get(kGpsPpsPin); }

bool Power::usb_plugged() const { return gpio_get(kUsbPluggedPin); }

bool Power::battery_charging() const { return !gpio_get(kChargingPin); }

float Power::battery_voltage() const {
  // The Pico board uses GPIO29 with a voltage divider to sample VSYS.
  // See Pico schematic.
  constexpr float kAdcVrefVoltage = 3.3f;
  constexpr float kAdcMax = 4096.0f;
  constexpr float kDividerResistorUpperKOhm = 200.0f;
  constexpr float kDividerResistorLowerKOhm = 100.0f;

  adc_select_input(3);  // Input 3 = GPIO 29
  float result = adc_read();
  float voltage = result * kAdcVrefVoltage / kAdcMax;
  float battery_voltage =
      voltage * (kDividerResistorUpperKOhm + kDividerResistorLowerKOhm) /
      kDividerResistorLowerKOhm;
  return battery_voltage;
}