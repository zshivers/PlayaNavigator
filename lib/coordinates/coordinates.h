#pragma once

#include <array>
#include <cstdint>

#include "maybe_valid.h"

constexpr double radToDeg(const double radians) {
  return radians * (180.0 / 3.14159265359);
}
constexpr double degToRad(const double deg) {
  return deg * (3.14159265359 / 180.0);
}
constexpr double metersToFeet(const double meters) { return meters * 3.281; }
constexpr double feetToMeters(const double feet) { return feet / 3.281; }
constexpr double feetToMiles(const double feet) { return feet / 5280.0; }
constexpr double metersToMiles(const double feet) { return feet / 1609.34; }

// Latitude and Longitude in decimal format.
// +Lat = N, -Lat = S
// +Lon = E, -Lon = W
struct LatLon {
  double lat, lon;
};

struct GpsInfo {
  uint32_t uart_time = 0;
  uint32_t update_time = 0;
  bool valid = false;
  uint32_t satellites = 0;
  LatLon location = {0.0, 0.0};
  double course_deg = 0.0;
  double speed_mph = 0.0;
  double hdop = 0.0;
  uint8_t hour = 0, minute = 0, second = 0;
};

// Important features that define the geometry of the map.
struct PlayaMapConfig {
  LatLon center;                // Central point all of BM is laid out around.
  double rotation_deg;          // Rotation from North to 12:00.
  struct RoadConfig {
    char road;        // Single letter for road.
    double radius_m;  // Radius from the center.
  };
  // Raods must be ordered in distance from man, from closest to furthest.
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
  double radius_m;
  MaybeValid<char> road;
};

// Returns true if the map configuration is valid.
bool PlayaMapConfigValid(const PlayaMapConfig& pmc);

PlayaCoords LatLonToPlayaCoords(const PlayaMapConfig& pmc, const LatLon& in);

// Does the supplied PlayaCoords have an address?
// The only parts of the playa that are addressable lie within an annulus
// between Esplanade and the last road, and are not between 10 o'clock and 2
// o'clock.
bool IsAddressable(const PlayaMapConfig& pmc, const PlayaCoords& pc);

PlayaAddress LatLonToAddress(const PlayaMapConfig& pmc,
                                         const LatLon& ll);

// Return the approximate distance in meters between two locations.
double distanceBetween(const LatLon& point1, const LatLon& point2);

// Return the course in degrees from point1 to point2.
double courseTo(const LatLon& point1, const LatLon& point2);