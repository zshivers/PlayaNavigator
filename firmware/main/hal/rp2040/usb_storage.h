#pragma once

#include <vector>

#include "mbed.h"  // Must be before "FlashIAPBlockDevice.h".
#include "FlashIAPBlockDevice.h"
#include "PluggableUSBMSD.h"
#include "coordinates.h"

class UsbStorage {
 public:
  UsbStorage();
  void Start();
  const PlayaMapConfig& GetPlayaMapConfig() {
    return usb_config_valid_ ? usb_map_config_ : default_map_config_;
  }
  const std::vector<LatLon>& GetBathrooms() { return bathrooms_; }

 private:
  mbed::FATFileSystem fs_;
  FlashIAPBlockDevice bd_;
  USBMSD usbmd_;
  FILE* map_file;

  bool usb_config_valid_ = false;
  PlayaMapConfig default_map_config_;
  PlayaMapConfig usb_map_config_;
  std::vector<LatLon> bathrooms_;
};
