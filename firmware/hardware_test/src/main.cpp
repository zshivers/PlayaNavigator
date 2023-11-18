#include <Arduino.h>
#include <stdio.h>

#include <cstdint>

#include "board.h"
#include "st7567.hpp"

UART gps_uart{/*tx=*/kGpsUartTxPin, /*rx=*/kGpsUartRxPin};

ST7567 display({
    .width = 128,
    .height = 64,
    .spi = spi0,
    .cs_pin = kDisplayCsPin,
    .sck_pin = kDisplaySckPin,
    .mosi_pin = kDisplayMosiPin,
    .dc_pin = kDisplayDcPin,
    .reset_pin = kDisplayResetPin,
});

void setup() {
  gpio_set_function(kPicoLedPin, GPIO_FUNC_SIO);
  gpio_set_dir(kPicoLedPin, GPIO_OUT);

  gps_uart.begin(9600);

  Serial.begin(115200);

  for (int pin :
       {kButtonAPin, kButtonBPin, kButtonCPin, kButtonDPin, kButtonEPin}) {
    gpio_set_function(pin, GPIO_FUNC_SIO);
    gpio_set_dir(pin, GPIO_IN);
    gpio_set_pulls(pin, true, false);
  }

  for (int pin : {kBacklightRedPin, kBacklightGreenPin, kBacklightBluePin,
                  kBacklightWhitePin}) {
    gpio_set_function(pin, GPIO_FUNC_SIO);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, false);
  }

  gpio_set_function(kPowerEnablePin, GPIO_FUNC_SIO);
  gpio_set_dir(kPowerEnablePin, GPIO_OUT);
  gpio_put(kPowerEnablePin, false);

  gpio_set_function(kGpsEnablePin, GPIO_FUNC_SIO);
  gpio_set_dir(kGpsEnablePin, GPIO_OUT);
  gpio_put(kGpsEnablePin, false);

  for (int pin : {kGpsPpsPin, kUsbPluggedPin}) {
    gpio_set_function(pin, GPIO_FUNC_SIO);
    gpio_set_dir(pin, GPIO_IN);
  }

  gpio_set_function(kChargingPin, GPIO_FUNC_SIO);
  gpio_set_dir(kChargingPin, GPIO_IN);
  gpio_set_pulls(kChargingPin, true, false);

  adc_init();
  adc_gpio_init(kVSysAdcInputPin);

  display.init();
}

class Periodic {
 public:
  Periodic(uint32_t period_ms) : period_ms_(period_ms), next_ms_(0) {}
  bool IsDue(uint32_t ms) {
    if (ms >= next_ms_) {
      next_ms_ = ms + period_ms_;
      return true;
    }
    return false;
  }

 private:
  uint32_t period_ms_;
  uint32_t next_ms_;
};

Periodic cycle_led(250);
Periodic cycle_backlight(1000);
Periodic cycle_display(100);

bool led = false;
int backlight_active = 0;
int gps_byte_count = 0;
int display_phase = 0;

void loop() {
  const uint32_t ms = millis();

  // Read battery voltage raw ADC.
  adc_select_input(3);
  const float vsys_adc_raw = adc_read();
  const float vsys = vsys_adc_raw * 3.3 / 4096 * ((200 + 100) / 100);

  // Read inputs.
  const bool button_a = gpio_get(kButtonAPin);
  const bool button_b = gpio_get(kButtonBPin);
  const bool button_c = gpio_get(kButtonCPin);
  const bool button_d = gpio_get(kButtonDPin);
  const bool button_e = gpio_get(kButtonEPin);
  const bool pps = gpio_get(kGpsPpsPin);
  const bool charging = gpio_get(kChargingPin);
  const bool usb_plugged = gpio_get(kUsbPluggedPin);

  // Count bytes received from the GPS.
  if (gps_uart.available() > 0) {
    gps_uart.read();
    gps_byte_count++;
  }

  char output[250];
  snprintf(output, sizeof(output), "A%d B%d C%d D%d E%d", button_a, button_b,
           button_c, button_d, button_e);
  Serial.println(output);

  snprintf(output, sizeof(output), "PPS:%d GPS Bytes:%d", pps, gps_byte_count);
  Serial.println(output);

  snprintf(output, sizeof(output), "Charging:%d USB Plugged:%d Vsys ADC: %d Vsys: %.2fV",
           charging, usb_plugged, static_cast<int>(vsys_adc_raw), vsys);
  Serial.println(output);

  Serial.println();

  // Blink LED on Pico board.
  if (cycle_led.IsDue(ms)) {
    led = !led;
    gpio_put(kPicoLedPin, led);
  }

  // Continuously cycle through all backlight colors.
  if (cycle_backlight.IsDue(ms)) {
    gpio_put(kBacklightRedPin, backlight_active == 0);
    gpio_put(kBacklightGreenPin, backlight_active == 1);
    gpio_put(kBacklightBluePin, backlight_active == 2);
    gpio_put(kBacklightWhitePin, backlight_active == 3);
    if (++backlight_active > 3) backlight_active = 0;
  }

  // Display a moving grid pattern.
  if (cycle_display.IsDue(ms)) {
    for (int x = 0; x < 128; ++x) {
      for (int y = 0; y < 64; ++y) {
        display.put_px(x, y, (x + display_phase) % 8 && (y + display_phase) % 8);
      }
    }
    display.update();
    if (++display_phase >= 8) display_phase = 0;
  }

  delay(10);
}