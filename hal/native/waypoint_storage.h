#pragma once

#include "coordinates.h"
#include "maybe_valid.h"

class WaypointStorage {
 public:
  static constexpr int kMaxWaypoints = 10;
  MaybeValid<LatLon> Read(int index);
  void Write(int index, LatLon waypoint);
  void EraseAll();

 private:
  struct SingleWaypoint {
    uint8_t valid;
    LatLon waypoint;
  };
  using WaypointStorageType =
      std::array<SingleWaypoint, WaypointStorage::kMaxWaypoints>;
  WaypointStorageType waypoints_;
};
