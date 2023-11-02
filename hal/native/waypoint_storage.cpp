#include "waypoint_storage.h"

#include "maybe_valid.h"

namespace {
constexpr uint8_t kValidKey = 0xAA;
}  // namespace

MaybeValid<LatLon> WaypointStorage::Read(int index) {
  if (index < 0 || index > kMaxWaypoints) {
    return MakeInvalid<LatLon>({});
  }

  if (index == 0) {
    return MakeValid<LatLon>({37.76986115865554, -122.41195171146316});
  }

  if (waypoints_[index].valid != kValidKey) return MakeInvalid<LatLon>({});

  return MakeValid<LatLon>(waypoints_[index].waypoint);
}

void WaypointStorage::Write(int index, LatLon waypoint) {
  if (index < 0 || index > kMaxWaypoints) return;
  waypoints_[index].waypoint = waypoint;
  waypoints_[index].valid = kValidKey;
}

void WaypointStorage::EraseAll() {
  for (auto waypoint : waypoints_) {
    waypoint.waypoint = LatLon{.lat = 0.0, .lon = 0.0};
    waypoint.valid = 0;
  }
}