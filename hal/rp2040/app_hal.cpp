#include <Arduino.h>

#include "Serial.h"
#include "hardware/spi.h"
#include "lvgl.h"
#include "pico/time.h"
#include "st7567.hpp"
#include "TinyGPS++.h"

constexpr int kDisplayWidth = 128;
constexpr int kDisplayHeight = 64;

ST7567::Options kDisplayOptions = {
    .width = kDisplayWidth,
    .height = kDisplayHeight,
    .spi = spi0,
    .cs_pin = 17,
    .sck_pin = 18,
    .mosi_pin = 19,
    .dc_pin = 20,
    .reset_pin = 21,
    .backlight_pin = 9,
};

ST7567 display(kDisplayOptions);
lv_disp_drv_t disp_drv;

UART gps_uart(4, 5);

TinyGPSPlus gps;

void display_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                      lv_color_t *color_p) {
  //   Serial.println("Flush: area");
  //   Serial.println(String(area->x1) + " " + String(area->y1));
  //   Serial.println(String(area->x2) + " " + String(area->y2));

  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      display.put_px(x, y, color_p->full > 0);
      color_p++;
    }
  }

  if (lv_disp_flush_is_last(disp_drv)) {
    display.update();
  }

  lv_disp_flush_ready(disp_drv);
}

void lv_log_cb(const char *buf) { Serial.println(String(buf, strlen(buf))); }

void hal_setup(void) {
  gpio_set_function(25, GPIO_FUNC_SIO);
  gpio_set_dir(25, GPIO_OUT);
  gpio_put(25, true);

  gps_uart.begin(115200);

  Serial.begin(115200);
  Serial.println("Setup");

  display.init();
  display.set_backlight(50);
  display.put_px(10, 10, true);
  display.update();

  lv_log_register_print_cb(lv_log_cb);

  lv_init();

  static lv_disp_draw_buf_t disp_buf;
  static lv_color_t buf_1[kDisplayWidth * 10];
  lv_disp_draw_buf_init(&disp_buf, buf_1, nullptr, kDisplayWidth * 10);

  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf;
  disp_drv.flush_cb = display_flush_cb;
  disp_drv.hor_res = kDisplayWidth;
  disp_drv.ver_res = kDisplayHeight;

  lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
}

void hal_loop(void) {
  static bool led = false;
  gpio_put(25, led);
  led ^= 1;

  lv_task_handler();
  lv_tick_inc(100);
  sleep_ms(100);
  
  while (gps_uart.available() > 0) {
    // Serial.print((char)gps_uart.read());
    gps.encode(gps_uart.read());
  }

  Serial.println("Number of sats: " + String(gps.satellites.value()));
  if (gps.time.isValid()) {
    Serial.println("Time: " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()));
  }
  Serial.println("");
}
