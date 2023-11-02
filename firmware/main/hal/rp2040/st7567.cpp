#include "st7567.hpp"

#include <cmath>

#include "hardware/gpio.h"
#include "hardware/spi.h"

enum reg : uint8_t {
  DISPOFF = 0xAE,         //
  DISPON = 0xAF,          //
  SETSTARTLINE = 0x40,    //
  REG_RATIO = 0x20,       //
  SETPAGESTART = 0xb0,    //
  PAGESTART_MASK = 0x07,  //
  SETCOLL = 0x00,         // 0x00-0x0f: Set lower column address
  SETCOLH = 0x10,         // 0x10-0x1f: Set higher column address
  SEG_DIR_NORMAL = 0xa0,  // 0xa0: Column address 0 is mapped to SEG0
  SEG_DIR_REV = 0xa1,     // 0xa1: Column address 128 is mapped to S
  DISPNORMAL = 0xa6,      // 0xa6: Normal display
  DISPINVERSE = 0xa7,     // 0xa7: Inverse display
  BIAS_1_9 = 0xa2,        // 0xa2: Select BIAS setting 1/9
  BIAS_1_7 = 0xa3,        //  # 0xa3: Select BIAS setting
  ENTER_RMWMODE = 0xe0,   // # 0xe0: Enter the Read Modify Write mode
  EXIT_RMWMODE = 0xee,    // # 0xee: Leave the Read Modify Write mode
  SETCOMNORMAL = 0xc0,    // # 0xc0: Set COM output direction, normal mode
  SETCOMREVERSE = 0xc8,   //  # 0xc8: Set COM output direction, reverse m
  POWERCTRL = 0x2f,       // Control built-in power circuit
  REG_RES_RR0 = 0x21,     // # 0x21: Regulation Resistior ratio
  REG_RES_RR1 = 0x22,     // # 0x22: Regulation Resistior ratio
  REG_RES_RR2 = 0x24,     //  # 0x24: Regulation Resistior ra
  SETCONTRAST = 0x81,     //  # 0x81: Set contrast cont
  SETBOOSTER = 0xf8,      // # Set booster level
  SETBOOSTER4X = 0x00,    // # Set booster level
  SETBOOSTER5X = 0x01,    //
  NOP = 0xe3,             // # 0xe3: NOP Command for no operation
};

void ST7567::reset() {
  gpio_put(options_.reset_pin, 0);
  sleep_ms(100);
  gpio_put(options_.reset_pin, 1);
  sleep_ms(100);
}

void ST7567::init() {
  // For some dumb reason, the Arduino framework modifies the Pico SDK to add
  // the leading underscore to this function!
  // spi_init(spi, spi_baud);
  _spi_init(options_.spi, options_.spi_baud);

  gpio_set_function(options_.reset_pin, GPIO_FUNC_SIO);
  gpio_set_dir(options_.reset_pin, GPIO_OUT);

  gpio_set_function(options_.dc_pin, GPIO_FUNC_SIO);
  gpio_set_dir(options_.dc_pin, GPIO_OUT);

  gpio_set_function(options_.cs_pin, GPIO_FUNC_SIO);
  gpio_set_dir(options_.cs_pin, GPIO_OUT);

  gpio_set_function(options_.sck_pin, GPIO_FUNC_SPI);
  gpio_set_function(options_.mosi_pin, GPIO_FUNC_SPI);

  reset();

  command(reg::BIAS_1_7);
  command(reg::SEG_DIR_NORMAL);
  command(reg::SETCOMREVERSE);
  command(reg::DISPNORMAL);
  command(reg::SETSTARTLINE | 0x00);  // Startline from 0x40-0x7F
  command(reg::POWERCTRL);
  command(reg::REG_RATIO | 4);
  command(reg::DISPON);
  command(reg::SETCONTRAST);
  command(30);
}

void ST7567::command(uint8_t command, size_t len, const uint8_t *data) {
  gpio_put(options_.cs_pin, 0);
  gpio_put(options_.dc_pin, 0);  // command mode
  spi_write_blocking(options_.spi, &command, 1);
  gpio_put(options_.cs_pin, 1);

  sleep_us(10);
  if (data) {
    gpio_put(options_.cs_pin, 0);
    gpio_put(options_.dc_pin, 1);  // data mode
    spi_write_blocking(options_.spi, data, len);
    gpio_put(options_.cs_pin, 1);
  }
}

void ST7567::put_px(int x, int y, bool on) {
  int b = x + (y / 8) * 128;
  if (!on)
    framebuffer_[b] |= 1 << (y % 8);
  else
    framebuffer_[b] &= ~(1 << (y % 8));
}

void ST7567::update() {
  for (uint8_t page = 0; page < 8; ++page) {
    command(reg::ENTER_RMWMODE);
    command(reg::SETPAGESTART | page);
    command(reg::SETCOLL);
    command(reg::SETCOLH);
    gpio_put(options_.cs_pin, 0);
    gpio_put(options_.dc_pin, 1);  // data mode
    spi_write_blocking(options_.spi, &framebuffer_[page * options_.width],
                       options_.width);
    gpio_put(options_.cs_pin, 1);
    gpio_put(options_.dc_pin, 0);  // Back to command mode
  }
  gpio_put(options_.cs_pin, 1);
}
