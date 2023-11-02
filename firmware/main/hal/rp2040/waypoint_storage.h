#pragma once

#include <Arduino.h>

#include "coordinates.h"
#include "maybe_valid.h"

class WaypointStorage {
 public:
  static constexpr int kMaxWaypoints = 10;
  MaybeValid<LatLon> Read(int index);
  void Write(int index, LatLon waypoint);
  void EraseAll();
};
