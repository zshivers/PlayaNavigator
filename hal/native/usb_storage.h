#pragma once

#include <vector>
#include "coordinates.h"

class UsbStorage {
 public:
  UsbStorage();
  void Start();
  const PlayaMapConfig& GetPlayaMapConfig() {
    return file_config_valid_ ? file_map_config_ : default_map_config_;
  }
  const std::vector<LatLon>& GetBathrooms() { return bathrooms_; }

 private:
  bool file_config_valid_ = false;
  PlayaMapConfig default_map_config_;
  PlayaMapConfig file_map_config_;
  std::vector<LatLon> bathrooms_;
};
