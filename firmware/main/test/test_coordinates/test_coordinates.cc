#include <gtest/gtest.h>

#include <cmath>

#include "coordinates.h"
#include "maybe_valid.h"

void PrintTo(const PlayaAddress& address, std::ostream* os) {
  *os << "(" << int(address.hour) << ":" << int(address.minute) << " & ";
  if (address.road.valid) {
    *os << address.road.value;
  } else {
    *os << metersToFeet(address.radius_m) << " ft";
  }
  *os << ")";
}

namespace {
constexpr double feetToMeters(const double feet) { return feet / 3.281; }

TEST(LatLonToPlayaCoords, Translation) {
  constexpr PlayaMapConfig kPlayaMapConfig = {
      .center = {.lat = 0.0, .lon = 0.0},
      .rotation_deg = 0.0,
  };
  // Using coordinates generated by https://www.gpsvisualizer.com/calculators
  // "Find the coordinates at a given distance and bearing", using
  // Starting Lat,Lon = 0,0; Distance = 1 km; Initial bearing: 45°
  PlayaCoords pc = LatLonToPlayaCoords(
      kPlayaMapConfig, {.lat = 0.00637334527, .lon = 0.00637334527});
  EXPECT_NEAR(pc.radius_m, 1000, 10);
  EXPECT_NEAR(pc.angle_deg, 45, 0.1);
}

TEST(LatLonToPlayaCoords, TranslationAndRotation) {
  constexpr PlayaMapConfig kPlayaMapConfig = {
      .center = {.lat = 0.0, .lon = 0.0},
      .rotation_deg = -45.0,
  };
  // Using the same coordinates as Translation test.
  PlayaCoords pc = LatLonToPlayaCoords(
      kPlayaMapConfig, {.lat = 0.00637334527, .lon = 0.00637334527});
  EXPECT_NEAR(pc.radius_m, 1000, 10);
  EXPECT_NEAR(pc.angle_deg, 0, 0.1);
}

// Test using points derived from BM's published map info for 2022, found here:
// https://bm-innovate.s3.amazonaws.com/2022/2022_BRC_Measurements.pdf
constexpr PlayaMapConfig kPlayaMapConfig = {
    .center = {.lat = 40.787030, .lon = -119.202740},
    // True north/south line is along 4:30
    // (360 deg / 12 h) * (6 - 4.5) = 45 deg
    .rotation_deg = -45.0,
    .roads = {{
        {'S', feetToMeters(2500)},
        {'A', feetToMeters(2940)},
        {'B', feetToMeters(3230)},
        {'C', feetToMeters(3520)},
        {'D', feetToMeters(3810)},
        {'E', feetToMeters(4100)},
        {'F', feetToMeters(4590)},
        {'G', feetToMeters(4880)},
        {'H', feetToMeters(5170)},
        {'I', feetToMeters(5460)},
        {'J', feetToMeters(5650)},
        {'K', feetToMeters(5840)},
    }},
};

// 2023 Map
constexpr PlayaMapConfig kPlaya2023MapConfig = {
    .center = {.lat = 40.786400, .lon = -119.203500},
    // True north/south line is along 4:30 = (360 deg / 12 h) * (6 - 4.5) = 45
    // deg
    .rotation_deg = -45.0,
    .roads = {{
        {'S', feetToMeters(2500)},
        {'A', feetToMeters(2940)},
        {'B', feetToMeters(3230)},
        {'C', feetToMeters(3520)},
        {'D', feetToMeters(3810)},
        {'E', feetToMeters(4100)},
        {'F', feetToMeters(4590)},
        {'G', feetToMeters(4880)},
        {'H', feetToMeters(5170)},
        {'I', feetToMeters(5460)},
        {'J', feetToMeters(5650)},
        {'K', feetToMeters(5845)},
    }},
};

// A simple configuration (one with no rotation and a zero center point).
constexpr PlayaMapConfig kSimpleMapConfig = {
    .center = {.lat = 0.0, .lon = 0.0},
    .rotation_deg = 0.0,
    .roads = {{
        {'S', feetToMeters(100)},
        {'A', feetToMeters(200)},
        {'B', feetToMeters(300)},
        {'C', feetToMeters(400)},
        {'D', feetToMeters(500)},
        {'E', feetToMeters(600)},
        {'F', feetToMeters(700)},
        {'G', feetToMeters(800)},
        {'H', feetToMeters(900)},
        {'I', feetToMeters(1000)},
        {'J', feetToMeters(1100)},
        {'K', feetToMeters(1200)},
    }},
};

TEST(PlayaMapConfigValid, ConfigInvalidIfRoadsAreNotMonoticallyIncreasing) {
  constexpr PlayaMapConfig kTestConfig = {
      .center = {.lat = 0.0, .lon = 0.0},
      .rotation_deg = -45.0,
      .roads = {{
          {'S', feetToMeters(100)},
          {'A', feetToMeters(200)},
          {'B', feetToMeters(300)},
          {'C', feetToMeters(400)},
          {'D', feetToMeters(500)},
          {'E', feetToMeters(700)},  // Wrong!
          {'F', feetToMeters(600)},
          {'G', feetToMeters(800)},
          {'H', feetToMeters(900)},
          {'I', feetToMeters(1000)},
          {'J', feetToMeters(1100)},
          {'K', feetToMeters(1200)},
      }}};
  EXPECT_FALSE(PlayaMapConfigValid(kTestConfig));
}

TEST(PlayaMapConfigValid, TestConfigValid) {
  EXPECT_TRUE(PlayaMapConfigValid(kPlayaMapConfig));
}

TEST(LatLonToPlayaCoords, BurningMan2022Map_P3) {
  PlayaCoords pc = LatLonToPlayaCoords(kPlayaMapConfig,
                                       {.lat = 40.803538, .lon = -119.181098});
  // P3 point is at 12:00.
  // Man to outer fence pentagon points: 8491’ = 2588.057 meters
  EXPECT_NEAR(pc.radius_m, feetToMeters(8491), 10);
  EXPECT_NEAR(pc.angle_deg, 0, 0.5);
}

TEST(LatLonToPlayaCoords, BurningMan2022Map_9andEsplanade) {
  // 9:00 & Espalande
  PlayaCoords pc = LatLonToPlayaCoords(kPlayaMapConfig,
                                       {.lat = 40.791875, .lon = -119.209115});
  EXPECT_NEAR(pc.radius_m, feetToMeters(2500), 10);
  EXPECT_NEAR(pc.angle_deg, -90, 2);
}

TEST(LatLonToPlayaCoords, BurningMan2022Map_3andEsplanade) {
  // 3:00 & Espalande
  PlayaCoords pc = LatLonToPlayaCoords(kPlayaMapConfig,
                                       {.lat = 40.782185, .lon = -119.196366});
  EXPECT_NEAR(pc.radius_m, feetToMeters(2500), 10);
  EXPECT_NEAR(pc.angle_deg, 90, 2);
}

TEST(IsAddressable, CloseToTheMan) {
  EXPECT_FALSE(
      IsAddressable(kPlayaMapConfig, {.radius_m = 80.0, .angle_deg = 5.0}));
}

TEST(IsAddressable, InnerPlaya) {
  EXPECT_FALSE(IsAddressable(
      kPlayaMapConfig, {.radius_m = feetToMeters(2400.0), .angle_deg = -90.0}));
}

TEST(IsAddressable, Between10and2) {
  EXPECT_FALSE(IsAddressable(
      kPlayaMapConfig, {.radius_m = feetToMeters(4000.0), .angle_deg = 0.0}));
}

TEST(IsAddressable, InCity) {
  // S & 2:00
  EXPECT_TRUE(IsAddressable(
      kPlayaMapConfig, {.radius_m = feetToMeters(2500.0), .angle_deg = 60.0}));
  // S & 10:00
  EXPECT_TRUE(IsAddressable(
      kPlayaMapConfig, {.radius_m = feetToMeters(2500.0), .angle_deg = -60.0}));
  // C & 3:00
  EXPECT_TRUE(IsAddressable(
      kPlayaMapConfig, {.radius_m = feetToMeters(3520.0), .angle_deg = 90.0}));
  // C & 9:00
  EXPECT_TRUE(IsAddressable(
      kPlayaMapConfig, {.radius_m = feetToMeters(3520.0), .angle_deg = -90.0}));
  // C & 6:00
  EXPECT_TRUE(IsAddressable(
      kPlayaMapConfig, {.radius_m = feetToMeters(3520.0), .angle_deg = 180.0}));
}

TEST(IsAddressable, PastLastRoad) {
  EXPECT_FALSE(IsAddressable(
      kPlayaMapConfig, {.radius_m = feetToMeters(5900.0), .angle_deg = 45.0}));
}

TEST(IsAddressable, DeepPlaya) {
  EXPECT_FALSE(IsAddressable(
      kPlayaMapConfig, {.radius_m = feetToMeters(6000.0), .angle_deg = 15.0}));
}

bool AddressEqual(const PlayaAddress& address1, const PlayaAddress& address2) {
  return address1.hour == address2.hour && address1.minute == address2.minute &&
         address1.road.valid == address2.road.valid &&
         address1.road.value == address2.road.value;
}

TEST(LatLonToAddress, DeepPlayaHasNoAddress) {
  // Deep playa around 12:00, midway between S and K.
  PlayaAddress address = LatLonToAddress(
      kPlayaMapConfig, {.lat = 40.7999025, .lon = -119.1827158});
  EXPECT_FALSE(address.road.valid);
}

TEST(LatLonToAddress, EsplanadeAnd9) {
  PlayaAddress address =
      LatLonToAddress(kPlayaMapConfig, {.lat = 40.791875, .lon = -119.209115});
  PlayaAddress expected = {
      .hour = 9, .minute = 0, .road = MakeValid<char>('S')};
  EXPECT_TRUE(AddressEqual(address, expected));
}

TEST(LatLonToAddress, Cand3) {
  PlayaAddress address =
      LatLonToAddress(kPlayaMapConfig, {.lat = 40.780208, .lon = -119.193766});
  PlayaAddress expected = {
      .hour = 3, .minute = 0, .road = MakeValid<char>('C')};
  EXPECT_TRUE(AddressEqual(address, expected));
}

TEST(LatLonToAddress, Jand6) {
  PlayaAddress address =
      LatLonToAddress(kPlayaMapConfig, {.lat = 40.776079, .lon = -119.217144});
  PlayaAddress expected = {
      .hour = 6, .minute = 0, .road = MakeValid<char>('J')};
  EXPECT_TRUE(AddressEqual(address, expected));
  PrintTo(address, &std::cout);
}

TEST(LatLonToAddress, Fand815) {
  PlayaAddress address =
      LatLonToAddress(kPlayaMapConfig, {.lat = 40.791843, .lon = -119.218033});
  PlayaAddress expected = {
      .hour = 8, .minute = 15, .road = MakeValid<char>('F')};
  EXPECT_TRUE(AddressEqual(address, expected));
}

// Check that the clock address is rounded to the nearest 5 minutes.
// Check locations in a ring every 1 minute on the clock.
TEST(LatLonToAddress, ClockRoundedTo5Mins) {
  constexpr double kRadiusFromMan_m = feetToMeters(1000.0);
  for (int hour = 0; hour < 12; ++hour) {
    for (int minute = 0; minute < 60; ++minute) {
      double clock_angle_deg = 360.0 * (hour + minute / 60.0) / 12.0;
      clock_angle_deg = 90 - clock_angle_deg;
      if (clock_angle_deg < -180) clock_angle_deg += 360;
      if (clock_angle_deg > 180) clock_angle_deg -= 360;

      const double angle_rad = degToRad(clock_angle_deg);
      const double delta_x_m = kRadiusFromMan_m * std::cos(angle_rad);
      const double delta_y_m = kRadiusFromMan_m * std::sin(angle_rad);

      const LatLon kLocation =
          GetOffsetLocation(kSimpleMapConfig.center, delta_x_m, delta_y_m);
      PlayaAddress address = LatLonToAddress(kSimpleMapConfig, kLocation);

      EXPECT_EQ(address.minute % 5, 0);  // Check rounded to 5 minutes.
    }
  }
}

TEST(LatLonToAddress2023, EsplanadeAnd10_2023) {
  PlayaAddress address = LatLonToAddress(
      kPlaya2023MapConfig, {.lat = 40.7929883, .lon = -119.2058906});
  PlayaAddress expected = {
      .hour = 10, .minute = 0, .road = MakeValid<char>('S')};
  PrintTo(address, &std::cout);
  EXPECT_TRUE(AddressEqual(address, expected));
}

TEST(LatLonToAddress2023, EsplanadeAnd2_2023) {
  PlayaAddress address = LatLonToAddress(
      kPlaya2023MapConfig, {.lat = 40.7845584, .lon = -119.1947878});
  PlayaAddress expected = {
      .hour = 2, .minute = 0, .road = MakeValid<char>('S')};
  EXPECT_TRUE(AddressEqual(address, expected));
  PrintTo(address, &std::cout);
}

TEST(LatLonToAddress2023, Cand3) {
  PlayaAddress address = LatLonToAddress(
      kPlaya2023MapConfig, {.lat = 40.7796292, .lon = -119.1945954});
  PlayaAddress expected = {
      .hour = 3, .minute = 0, .road = MakeValid<char>('C')};
  EXPECT_TRUE(AddressEqual(address, expected));
  PrintTo(address, &std::cout);
}

TEST(LatLonToAddress2023, Jand6) {
  PlayaAddress address = LatLonToAddress(
      kPlaya2023MapConfig, {.lat = 40.7754071, .lon = -119.2177962});
  PlayaAddress expected = {
      .hour = 6, .minute = 0, .road = MakeValid<char>('J')};
  EXPECT_TRUE(AddressEqual(address, expected));
  PrintTo(address, &std::cout);
}

TEST(LatLonToAddress2023, Fand815) {
  PlayaAddress address = LatLonToAddress(
      kPlaya2023MapConfig, {.lat = 40.7914487, .lon = -119.2192866});
  PlayaAddress expected = {
      .hour = 8, .minute = 15, .road = MakeValid<char>('F')};
  EXPECT_TRUE(AddressEqual(address, expected));
  PrintTo(address, &std::cout);
}

TEST(GetOffsetLocation, OffsetWithKnownCoordinates45Deg) {
  // Web tool used below treats bearing relative to north.
  // Bearing on a unit circle is relative to +x axis. Hence 90 - X
  // transformation.
  LatLon location = GetOffsetLocation({.lat = 0.0, .lon = 0.0},
                                      1000.0 * std::cos(degToRad(90.0 - 45.0)),
                                      1000.0 * std::sin(degToRad(90.0 - 45.0)));

  // Approximately 20 meters.
  constexpr double kTolerance_deg = 20.0 / 111111.0;

  // Using coordinates generated by https://www.gpsvisualizer.com/calculators
  // "Find the coordinates at a given distance and bearing", using
  // Starting Lat,Lon = 0,0; Distance = 1 km; Initial bearing: 45°
  EXPECT_NEAR(location.lat, 0.00637334527, kTolerance_deg);
  EXPECT_NEAR(location.lon, 0.00637334527, kTolerance_deg);
}

TEST(GetOffsetLocation, OffsetWithKnownCoordinates30Deg) {
  // Web tool used below treats positive bearing relative to north.
  // Bearing on a unit circle is relative to +x axis. Hence 90 - X
  // transformation.
  LatLon location = GetOffsetLocation({.lat = 0.0, .lon = 0.0},
                                      1000.0 * std::cos(degToRad(90.0 - 30.0)),
                                      1000.0 * std::sin(degToRad(90.0 - 30.0)));

  // Approximately 20 meters.
  constexpr double kTolerance_deg = 20.0 / 111111.0;

  // Using coordinates generated by https://www.gpsvisualizer.com/calculators
  // "Find the coordinates at a given distance and bearing", using
  // Starting Lat,Lon = 0,0; Distance = 1 km; Initial bearing: 45°
  EXPECT_NEAR(location.lat, 0.00781884223, kTolerance_deg);
  EXPECT_NEAR(location.lon, 0.00451425674, kTolerance_deg);
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // if you plan to use GMock, replace the line above with
  // ::testing::InitGoogleMock(&argc, argv);

  if (RUN_ALL_TESTS())
    ;

  // Always return zero-code and allow PlatformIO to parse results
  return 0;
}