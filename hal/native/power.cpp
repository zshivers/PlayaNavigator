#include "power.h"

void Power::Start() {}

void Power::power_enable(bool on) {}

void Power::gps_enable(bool on) {}

bool Power::gps_pps() const { return false; }

bool Power::battery_charging() { return false; }

float Power::battery_voltage() { return 3.8; }

bool Power::usb_plugged() { return false; }