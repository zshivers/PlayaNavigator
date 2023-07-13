#pragma once

class Power {
 public:
  void Start();
  float battery_voltage();
  bool battery_charging();
  void power_enable(bool on);
  void gps_enable(bool on);
  bool gps_pps() const;
  bool usb_plugged();
};
