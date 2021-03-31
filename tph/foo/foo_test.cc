#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "tph/foo/foo.h"

TEST_CASE("foo_test") {
  REQUIRE(tph::FooFunc("hello") == 5);
}

TEST_CASE("fftwf") {

  REQUIRE(tph::MyFftw() == 0);
}
