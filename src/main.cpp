#include "app_hal.h"
#include "ui.h"
#include "button.h"
#include "lvgl.h"

Ui* ui;
Button* button_a;
Button* button_b;
Button* button_c;
Button* button_d;
Button* button_e;

#ifdef PLATFORM_ARDUINO
#include <Arduino.h>
#endif

void setup() {
  hal_setup();
  ui = new Ui();
  button_a = new Button(12);
  button_b = new Button(13);
  button_c = new Button(14);
  button_d = new Button(15);
  button_e = new Button(22);
}

void loop() {
  hal_loop();
  if (button_a->read()) {
    ui->SetMode(Ui::Mode::kLocation);
  } else if (button_b->read()) {
    ui->SetMode(Ui::Mode::kWaypoint);
  }
}

#ifdef PLATFORM_NATIVE
int main(void) {
  setup();
  while (true) {
    loop();
  }
}
#endif
