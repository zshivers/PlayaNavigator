#pragma once

#include <cstdint>

enum Polarity { ACTIVE_LOW = 0, ACTIVE_HIGH = 1 };

class Button {
 public:
  Button(uint8_t pin, Polarity polarity = Polarity::ACTIVE_LOW,
         uint32_t repeat_time = 200, uint32_t hold_time = 1000)
      : repeat_time(repeat_time),
        hold_time(hold_time) {

  };
  bool raw() { return false; }
  bool read() { return false; }

 private:
  uint32_t repeat_time;
  uint32_t hold_time;
  bool pressed = false;
  bool last_state = false;
  uint32_t pressed_time = 0;
  uint32_t last_time = 0;
};
