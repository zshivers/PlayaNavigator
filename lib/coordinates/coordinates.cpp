#include "coordinates.h"

#include <cmath>
#include <iostream>

#include "maybe_valid.h"

namespace {
inline double square(double in) { return in * in; }

constexpr double kRoadBoundaryPadding_m = feetToMeters(50);

double CityInnerRadialBoundary(const PlayaMapConfig& pmc) {
  return pmc.roads.front().radius_m - kRoadBoundaryPadding_m;
}

double CityOuterRadialBoundary(const PlayaMapConfig& pmc) {
  return pmc.roads.back().radius_m + kRoadBoundaryPadding_m;
}
}  // namespace

double distanceBetween(const LatLon& point1, const LatLon& point2) {
  double lat1 = point1.lat;
  double lat2 = point2.lat;
  double long1 = point1.lon;
  double long2 = point2.lon;
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6372795 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  constexpr double kEarthRadius = 6372795.0;
  double delta = degToRad(long1 - long2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = degToRad(lat1);
  lat2 = degToRad(lat2);
  const double slat1 = sin(lat1);
  const double clat1 = cos(lat1);
  const double slat2 = sin(lat2);
  const double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = square(delta);
  delta += square(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * kEarthRadius;
}

double courseTo(const LatLon& point1, const LatLon& point2) {
  const double lat1 = degToRad(point1.lat);
  const double lat2 = degToRad(point2.lat);
  const double long1 = point1.lon;
  const double long2 = point2.lon;
  // returns course in degrees (North=0, West=-90, East=+90) from position 1 to
  // position 2, both specified as signed decimal-degrees latitude and
  // longitude. Because Earth is no exact sphere, calculated course may be off
  // by a tiny fraction. Courtesy of Maarten Lamers
  double dlon = degToRad(long2 - long1);
  double y = sin(dlon) * cos(lat2);
  double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dlon);
  return radToDeg(atan2(y, x));
}

bool PlayaMapConfigValid(const PlayaMapConfig& pmc) {
  // Center point is a valid lat/lon.
  if (pmc.center.lat < -90.0 || pmc.center.lat > 90.0 ||
      pmc.center.lon < -180.0 || pmc.center.lon > 180.0) {
    return false;
  }

  // Check the roads.
  double last_radius_m = -1.0;
  for (const auto road : pmc.roads) {
    // All radii must be positive.
    if (road.radius_m < 0) return false;
    // Radii in list must be monotonically increasing.
    if (road.radius_m < last_radius_m) return false;
    last_radius_m = road.radius_m;
    // Need a letter for the road label.
    if (!isalpha(road.road)) return false;
  }

  // Rotation degrees must be in [-360, 360].
  if (pmc.rotation_deg < -360.0 || pmc.rotation_deg > 360.0) return false;
  
  return true;
}

PlayaCoords LatLonToPlayaCoords(const PlayaMapConfig& pmc, const LatLon& in) {
  PlayaCoords out;
  out.radius_m = distanceBetween(pmc.center, in);
  out.angle_deg = courseTo(pmc.center, in) + pmc.rotation_deg;
  return out;
}

bool IsAddressable(const PlayaMapConfig& pmc, const PlayaCoords& pc) {
  constexpr double kTenOClock_deg = (360.0 / 12) * -2.0;
  constexpr double kTwoOClock_deg = (360.0 / 12) * 2.0;
  return pc.radius_m >= CityInnerRadialBoundary(pmc) &&
         pc.radius_m <= CityOuterRadialBoundary(pmc) &&
         !(pc.angle_deg > kTenOClock_deg && pc.angle_deg < kTwoOClock_deg);
}

PlayaAddress LatLonToAddress(const PlayaMapConfig& pmc, const LatLon& ll) {
  const PlayaCoords pc = LatLonToPlayaCoords(pmc, ll);

  PlayaAddress address;
  double clock_hours = pc.angle_deg * 12.0 / 360.0;
  if (clock_hours < 0.0) clock_hours += 12.0;

  // Round to nearest 5 minutes.
  constexpr double kRoundingDenom = 60 / 5;
  clock_hours = std::round(clock_hours * kRoundingDenom) / kRoundingDenom;

  // Split into hours and minutes.
  double clock_integral;
  const double clock_fractional = std::modf(clock_hours, &clock_integral);
  address.hour = std::round(clock_integral);
  if (address.hour == 0) address.hour = 12;
  address.minute = std::round(clock_fractional * 60.0);
  if (address.minute == 60) address.minute = 0;

  address.radius_m = pc.radius_m;

  if (!IsAddressable(pmc, pc)) {
    address.road = MakeInvalid<char>(' ');
    return address;
  }

  // Find the nearest road.
  for (size_t i = 0; i < pmc.roads.size(); ++i) {
    double r_before =
        i == 0 ? CityInnerRadialBoundary(pmc) : pmc.roads[i].radius_m;
    double r_after = i < (pmc.roads.size() - 1) ? pmc.roads[i + 1].radius_m
                                                : CityOuterRadialBoundary(pmc);
    const double r_mid = (r_before + r_after) / 2.0;
    if (pc.radius_m < r_mid) {
      address.road = MakeValid<char>(pmc.roads[i].road);
      break;
    }
  }

  return address;
}

LatLon GetOffsetLocation(const LatLon& center, double delta_x_meters,
                         double delta_y_meters) {
  // https://gis.stackexchange.com/questions/2951/algorithm-for-offsetting-a-latitude-longitude-by-some-amount-of-meters
  return {.lat = center.lat + delta_y_meters / 111111.0,
          .lon = center.lon + delta_x_meters / (111111.0 * std::cos(degToRad(center.lon)))};
}