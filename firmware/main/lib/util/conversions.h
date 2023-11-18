#pragma once

constexpr double radToDeg(const double radians) {
  return radians * (180.0 / 3.14159265359);
}
constexpr double degToRad(const double deg) {
  return deg * (3.14159265359 / 180.0);
}
constexpr double metersToFeet(const double meters) { return meters * 3.281; }
constexpr double feetToMeters(const double feet) { return feet / 3.281; }
constexpr double feetToMiles(const double feet) { return feet / 5280.0; }
constexpr double milesToFeet(const double miles) { return miles * 5280.0; }
constexpr double metersToMiles(const double feet) { return feet / 1609.34; }
