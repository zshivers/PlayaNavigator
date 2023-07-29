#include <gtest/gtest.h>

#include "auto_shutdown.h"

namespace {
TEST(AutoShutdown, Test1) {
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