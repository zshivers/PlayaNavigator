#include <Arduino.h>

#include "Serial.h"
#include "board.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pll.h"
#include "hardware/spi.h"
#include "lvgl.h"
#include "pico/time.h"
#include "power.h"
#include "st7567.hpp"

constexpr int kDisplayWidth = 128;
constexpr int kDisplayHeight = 64;

ST7567::Options kDisplayOptions = {
    .width = kDisplayWidth,
    .height = kDisplayHeight,
    .spi = spi0,
    .cs_pin = kDisplayCsPin,
    .sck_pin = kDisplaySckPin,
    .mosi_pin = kDisplayMosiPin,
    .dc_pin = kDisplayDcPin,
    .reset_pin = kDisplayResetPin,
};

ST7567 display(kDisplayOptions);
lv_disp_drv_t disp_drv;

void display_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                      lv_color_t *color_p) {
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

void clock_setup() {
  // Change clk_sys to be 48MHz. The simplest way is to take this from PLL_USB
  // which has a source frequency of 48MHz
  clock_configure(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                  CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB, 48 * MHZ,
                  48 * MHZ);

  // Turn off PLL sys for good measure
  pll_deinit(pll_sys);

  // CLK peri is clocked from clk_sys so need to change clk_peri's freq
  clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                  48 * MHZ, 48 * MHZ);
}

void hal_setup(void) {
  // clock_setup();

  // Built-in LED off to save power.
  gpio_set_function(kPicoLedPin, GPIO_FUNC_SIO);
  gpio_set_dir(kPicoLedPin, GPIO_OUT);
  gpio_put(kPicoLedPin, false);

  Serial.begin(115200);
  Serial.println("Playa Naviator GPS");

  display.init();

  // lv_log_register_print_cb(lv_log_cb);

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
  lv_task_handler();
  lv_tick_inc(5);
  sleep_ms(5);
}
