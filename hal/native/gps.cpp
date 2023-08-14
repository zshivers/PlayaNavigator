#include "gps.h"

#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

void Gps::Start() {
  static auto io_thread = std::thread([&] {
    std::string s;
    while (!error && std::getline(std::cin, s, '\n')) {
      double lat, lon;
      int n = sscanf(s.c_str(), "%lf,%lf", &lat, &lon);
      if (n == 2) {
        auto lock = std::unique_lock<std::mutex>(m);
        gps_info_.location.lat = lat;
        gps_info_.location.lon = lon;
        gps_info_.valid = true;
        lock.unlock();
      }
    }
    auto lock = std::unique_lock<std::mutex>(m);
    error = true;
    lock.unlock();
  });
}

GpsInfo Gps::gps_info() {
  GpsInfo t;
  auto lock = std::unique_lock<std::mutex>(m);
  t = gps_info_;
  lock.unlock();
  return t;
}