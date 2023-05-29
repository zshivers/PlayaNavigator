
#include "waypoint_storage.h"

#include <hardware/flash.h>
#include <hardware/sync.h>
#include "maybe_valid.h"

namespace {
constexpr uint8_t kValidKey = 0xAA;
constexpr uint32_t kFlashOffset = 0x100000;
constexpr size_t kSectorSize = 4096;  // Minimum bytes to erase at once.
constexpr size_t kPageSize = 256;     // Minimimum bytes to program at once.
struct SingleWaypoint {
  uint8_t valid;
  LatLon waypoint;
};
using WaypointStorageType =
    std::array<SingleWaypoint, WaypointStorage::kMaxWaypoints>;
static_assert(sizeof(WaypointStorageType) <= kPageSize);
}  // namespace

MaybeValid<LatLon> WaypointStorage::Read(int index) {
  if (index < 0 || index > kMaxWaypoints) {
    return MakeInvalid<LatLon>({});
  }

  const WaypointStorageType& waypoints =
      *reinterpret_cast<WaypointStorageType*>(XIP_BASE + kFlashOffset);
  
  if (waypoints[index].valid != kValidKey)
    return MakeInvalid<LatLon>({});

  return MakeValid<LatLon>(waypoints[index].waypoint);
}

void WaypointStorage::Write(int index, LatLon waypoint) {
  if (index < 0 || index > kMaxWaypoints) return;

  // Get a RAM copy of the waypoints from Flash.
  const WaypointStorageType* waypoints =
      reinterpret_cast<WaypointStorageType*>(XIP_BASE + kFlashOffset);
  WaypointStorageType waypoints_copy;
  memcpy(&waypoints_copy, waypoints, sizeof(waypoints_copy));
  // Modify the point to be written.
  waypoints_copy[index].waypoint = waypoint;
  waypoints_copy[index].valid = kValidKey;

  // Write entire set of waypoints to Flash.
  noInterrupts();
  flash_range_erase(kFlashOffset, kSectorSize);
  flash_range_program(kFlashOffset, (uint8_t*)&waypoints_copy,
                      sizeof(waypoints_copy));
  interrupts();
}

void WaypointStorage::EraseAll() {
  noInterrupts();
  flash_range_erase(kFlashOffset, kSectorSize);
  interrupts();
}