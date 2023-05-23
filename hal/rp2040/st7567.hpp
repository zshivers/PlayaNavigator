#pragma once

#include <cstdint>

#include "hardware/pwm.h"
#include "hardware/spi.h"

class ST7567 {
 public:
  struct Options {
    uint16_t width;
    uint16_t height;
    spi_inst_t *spi;
    uint32_t spi_baud = 10'000'000;
    uint cs_pin;
    uint sck_pin;
    uint mosi_pin;
    uint dc_pin;
    uint reset_pin;
    uint backlight_pin;
  };

  ST7567(Options options) : options_(options) {}
  void init();
  void update();
  void put_px(int x, int y, bool on);
  void set_backlight(uint8_t brightness);
  void reset();

 private:
  Options options_;
  uint8_t framebuffer_[128 * 8] = {0};

  void command(uint8_t command, size_t len = 0, const uint8_t *data = nullptr);
};
