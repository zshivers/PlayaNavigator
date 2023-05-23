#include "gps.h"

#include <Arduino.h>

Gps::Gps() {
  Serial1.begin(115200);    
}
