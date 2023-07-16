#include "OneButton.h"
#include "app_hal.h"
#include "backlight.h"
#include "button.h"
#include "gps.h"
#include "hal_time.h"
#include "lvgl.h"
#include "maybe_valid.h"
#include "power.h"
#include "ui.h"
#include "waypoint_storage.h"

// #include "pico/time.h"
#ifdef PLATFORM_NATIVE
#include <iostream>
#endif

Ui* ui;
Gps* gps;
Power power;
Backlight* backlight;
OneButton button_a(12);
OneButton button_b(13);
OneButton button_c(14);
OneButton button_d(15);
OneButton button_e(22);

#ifdef PLATFORM_ARDUINO
#include <Arduino.h>
#endif

void button_a_press_cb() { ui->ButtonPress(Ui::Button::kA); }
void button_b_press_cb() { ui->ButtonPress(Ui::Button::kB); }
void button_c_press_cb() { ui->ButtonPress(Ui::Button::kC); }
void button_d_press_cb() { ui->ButtonPress(Ui::Button::kD); }
void button_e_press_cb() { ui->ButtonPress(Ui::Button::kE); }

void button_a_long_press_cb() { ui->ButtonLongPress(Ui::Button::kA); }
void button_b_long_press_cb() { ui->ButtonLongPress(Ui::Button::kB); }
void button_c_long_press_cb() { ui->ButtonLongPress(Ui::Button::kC); }
void button_d_long_press_cb() { ui->ButtonLongPress(Ui::Button::kD); }
void button_e_long_press_cb() { ui->ButtonLongPress(Ui::Button::kE); }

void setup() {
  power.Start();

  // Since the pushbutton for power will only keep the power on until the user
  // releases it, the first thing to do on boot is to take over control to keep
  // power on.
  power.power_enable(true);

  // Immediately shutdown if battery voltage is too low.
  // if (battery_voltage() < 3.2) {
  //   power_enable(false);
  //   sleep_ms(1000);
  // }

  power.gps_enable(true);

  hal_setup();

  backlight = new Backlight(9, 6, 7, 8);
  backlight->SetColor(255, 0, 0, 0);
  ui = new Ui(power, backlight);
  gps = new Gps();

  button_a.attachClick(button_a_press_cb);
  button_b.attachClick(button_b_press_cb);
  button_c.attachClick(button_c_press_cb);
  button_d.attachClick(button_d_press_cb);
  button_e.attachClick(button_e_press_cb);

  button_a.attachLongPressStart(button_a_long_press_cb);
  button_b.attachLongPressStart(button_b_long_press_cb);
  button_c.attachLongPressStart(button_c_long_press_cb);
  button_d.attachLongPressStart(button_d_long_press_cb);
  button_e.attachLongPressStart(button_e_long_press_cb);
}

void loop() {
  hal_loop();

  button_a.tick();
  button_b.tick();
  button_c.tick();
  button_d.tick();
  button_e.tick();

  const uint32_t ms = time_millis();
  gps->Update();

  // const absolute_time_t start = get_absolute_time();
  ui->OnGpsInfo(gps->gps_info());
  ui->update(ms);
  // const absolute_time_t end = get_absolute_time();

  // Serial.println(absolute_time_diff_us(start, end));
}

#ifdef PLATFORM_NATIVE
int main(void) {
  setup();
  while (true) {
    loop();
  }
}
#endif
