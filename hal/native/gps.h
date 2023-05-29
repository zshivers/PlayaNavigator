#pragma once

#include "coordinates.h"

class Gps {
public:
    Gps() {};
    void update() {}
    GpsInfo gps_info() const { return gps_info_; }
private:
    GpsInfo gps_info_;
};