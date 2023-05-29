#include "coordinates.h"

#include <cmath>

#include "maybe_valid.h"

namespace {
inline double square(double in) { return in * in; }

int RoundUpToNearestMultiple(int in, int multiple) {
  if (multiple == 0) return in;

  int remainder = in % multiple;
  if (remainder == 0) return in;

  return in + multiple - remainder;
}
}  // namespace

double distanceBetween(LatLon point1, LatLon point2) {
  double& lat1 = point1.lat;
  double& lat2 = point2.lat;
  double& long1 = point1.lon;
  double& long2 = point2.lon;
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

double courseTo(LatLon point1, LatLon point2) {
  double& lat1 = point1.lat;
  double& lat2 = point2.lat;
  double& long1 = point1.lon;
  double& long2 = point2.lon;
  // returns course in degrees (North=0, West=-90, East=+90) from position 1 to
  // position 2, both specified as signed decimal-degrees latitude and
  // longitude. Because Earth is no exact sphere, calculated course may be off
  // by a tiny fraction. Courtesy of Maarten Lamers
  double dlon = degToRad(long2 - long1);
  lat1 = degToRad(lat1);
  lat2 = degToRad(lat2);
  double y = sin(dlon) * cos(lat2);
  double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dlon);
  return radToDeg(atan2(y, x));
}

PlayaCoords LatLonToPlayaCoords(const PlayaMapConfig& pmc, const LatLon& in) {
  PlayaCoords out;
  out.radius_m = distanceBetween(pmc.center, in);
  out.angle_deg = courseTo(pmc.center, in) + pmc.rotation_deg;
  return out;
}

bool IsAddressable(const PlayaMapConfig& pmc, const PlayaCoords& pc) {
  constexpr double angle_10_deg = (360.0 / 12) * -2.0;
  constexpr double angle_2_deg = (360.0 / 12) * 2.0;
  return pc.radius_m > pmc.esplanade_radius_m &&
         pc.radius_m < pmc.last_road_radius_m &&
         !(pc.angle_deg > angle_10_deg && pc.angle_deg < angle_2_deg);
}

MaybeValid<PlayaAddress> LatLonToAddress(const PlayaMapConfig& pmc,
                                         const LatLon& ll) {
  const PlayaCoords pc = LatLonToPlayaCoords(pmc, ll);

  PlayaAddress address;
  double clock_decimal = pc.angle_deg / 360.0 * 12.0;
  if (clock_decimal < 0.0) clock_decimal += 12.0;
  address.hour = static_cast<uint8_t>(clock_decimal);
  if (address.hour == 0) address.hour = 12;
  address.minute = static_cast<uint8_t>((clock_decimal - address.hour) * 60);
  address.minute =
      RoundUpToNearestMultiple(address.minute, 5);  // Round to nearest 5 mins.
  if (address.minute == 60) address.minute = 0;

  address.radius_m = pc.radius_m;

  if (!IsAddressable(pmc, pc)) {
    address.road = MakeInvalid<char>(' ');
    return MakeValid<PlayaAddress>(address);
  }

  constexpr double kBoundaryPadding_m = 50;
  for (int i = 0; i < pmc.roads.size(); ++i) {
    double r_before = i == 0 ? pmc.roads[0].radius_m - kBoundaryPadding_m
                             : pmc.roads[i].radius_m;
    double r_after = i < (pmc.roads.size() - 1)
                         ? r_after = pmc.roads[i + 1].radius_m
                         : r_after = r_before + kBoundaryPadding_m;
    const double r_mid = (r_before + r_after) / 2.0;
    if (pc.radius_m < r_mid) {
      address.road = MakeValid<char>(pmc.roads[i].road);
      break;
    }
  }

  return MakeValid<PlayaAddress>(address);
}