#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "tph/bar/bar.h"

TEST_CASE("bar_test") {
  REQUIRE(tph::BarFunc("hello") == (42 - 5));
}
