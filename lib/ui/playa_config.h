#pragma once

#include "coordinates.h"

#if 0
constexpr PlayaMapConfig kPlayaMapConfig = {
    .center = {.lat = 40.787030, .lon = -119.202740},
    // True north/south line is along 4:30 = (360 deg / 12 h) * (6 - 4.5) = 45
    // deg
    .rotation_deg = -45.0,
    .esplanade_radius_m = feetToMeters(2500.0),
    .last_road_radius_m = feetToMeters(11690.0 / 2.0),  // 5845
    .trash_fence_radius_m = feetToMeters(8491.0),
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

constexpr std::array<LatLon, 5> kBathroomLocations = {{
    {40.787030, -119.202740},
    {40.787030, -119.202740},
    {40.787030, -119.202740},
    {40.787030, -119.202740},
    {40.787030, -119.202740},
}};
#endif

// SF local config for testing!
constexpr PlayaMapConfig kPlayaMapConfig = {
    .center = {.lat = 37.776183400365476, .lon = -122.41379170684557},
    .rotation_deg = -45.0,
    .esplanade_radius_m = 71.0,
    .last_road_radius_m = 1000.0,
    .trash_fence_radius_m = 1850.0,
    .roads = {{
        {'S', 71.0},  // Natoma
        {'A', 137.0}, // Howard
        {'B', 204.0}, // Tehama
        {'C', 262.0}, // Clementina
        {'D', 329.9}, // Folsom
        {'E', 398.0}, // Ringold
        {'F', 500.0},
        {'G', 600.0},
        {'H', 700.0},
        {'I', 800.0},
        {'J', 900.0},
        {'K', 1000.0},
    }},
};

constexpr std::array<LatLon, 3> kBathroomLocations = {{
    {37.775913255761346, -122.41415061528251},  // 9th & Minna.
    {37.775291235690105, -122.41572152148551},  // 10th & Mission
    {37.77483219054346, -122.41139189301217}, // Middle of clementina
}};