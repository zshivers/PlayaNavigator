#pragma once

#include <array>
#include <cstdint>
// #include <optional>

// Latitude and Longitude in decimal format.
// +Lat = N, -Lat = S
// +Lon = E, -Lon = W
struct LatLon {
  double lat, lon;
};

// Important features that define the geometry of the map.
struct PlayaMapConfig {
  LatLon center;                // Central point all of BM is laid out around.
  double rotation_deg;          // Rotation from North to 12:00.
  double esplanade_radius_m;    // Most inner road.
  double last_road_radius_m;    // Most outer road.
  double trash_fence_radius_m;  // Radius of circle circumscribing trash fench
                                // pentagon.
  struct RoadConfig {
    char road;        // Single letter for road.
    double radius_m;  // Radius from the center.
  };
  std::array<RoadConfig, 12> roads;
};

// Coordinates relative to the center of the playa.
// angle_deg ==   0 -> 12:00
// angle_deg == +90 ->  3:00
// angle_deg == -90 ->  9:00
// Radius is the distance from the center point (the man).
struct PlayaCoords {
  double radius_m, angle_deg;
};

struct PlayaAddress {
  uint8_t hour;
  uint8_t minute;
  char road;

  friend bool operator==(const PlayaAddress& lhs, const PlayaAddress& rhs) {
    return lhs.hour == rhs.hour && lhs.minute == rhs.minute &&
           lhs.road == rhs.road;
  }
};

PlayaCoords LatLonToPlayaCoords(const PlayaMapConfig& pmc, const LatLon& in);

// Does the supplied PlayaCoords have an address?
// The only parts of the playa that are addressable lie within an annulus
// between Esplanade and the last road, and are not between 10 o'clock and 2
// o'clock.
bool IsAddressable(const PlayaMapConfig& pmc, const PlayaCoords& pc);

// std::optional<PlayaAddress> PlayaCoordsToAddress(const PlayaMapConfig& pmc,
//                                                  const PlayaCoords& pc);