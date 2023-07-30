#pragma once

class Power {
 public:
  void Start();
  float battery_voltage() const;
  bool battery_charging() const;
  void power_enable(bool on);
  void gps_enable(bool on);
  bool gps_pps() const;
  bool usb_plugged() const;
  float temperature() const;
};
