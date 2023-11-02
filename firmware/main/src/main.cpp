#include "OneButton.h"
#include "app_hal.h"
#include "backlight.h"
#include "board.h"
#include "gps.h"
#include "hal_time.h"
#include "lvgl.h"
#include "maybe_valid.h"
#include "power.h"
#include "ui.h"
#include "usb_storage.h"
#include "waypoint_storage.h"

Ui* ui;
Gps gps;
Power power;
Backlight* backlight;
UsbStorage usb_storage;
OneButton button_a(kButtonAPin);
OneButton button_b(kButtonBPin);
OneButton button_c(kButtonCPin);
OneButton button_d(kButtonDPin);
OneButton button_e(kButtonEPin);

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

  power.gps_enable(true);

  hal_setup();

  usb_storage.Start();

  backlight = new Backlight(kBacklightWhitePin, kBacklightRedPin,
                            kBacklightGreenPin, kBacklightBluePin);
  backlight->SetColor(1.0, 0, 0, 0);
  ui = new Ui(usb_storage.GetPlayaMapConfig(), usb_storage.GetBathrooms(),
              power, backlight);

  gps.Start();

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
  gps.Update(ms);

  ui->OnGpsInfo(gps.gps_info());
  ui->update(ms);
}

#ifdef PLATFORM_NATIVE
int main(void) {
  setup();
  while (true) {
    loop();
  }
}
#endif
