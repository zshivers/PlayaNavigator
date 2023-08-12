#include "gps.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>

#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

constexpr int kUdpPort = 8000;

void Gps::Start() {
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("cannot create socket\n");
    return;
  }

  struct sockaddr_in myaddr;
  memset((char *)&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(kUdpPort);

  if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    perror("bind failed");
    return;
  }

  static auto io_thread = std::thread([&] {
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    while (true) {
      int recvlen = recvfrom(fd, buf, 2048, 0, (struct sockaddr *)&remaddr, &addrlen);
      if (recvlen <= 0) continue;

      double lat, lon;
      int n = sscanf((char*)buf, "%lf,%lf", &lat, &lon);
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