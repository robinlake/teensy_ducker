#include "../examples/demo.h"
// #include "../lib/Catch2/single_include/catch2/catch.hpp"
#include "../lib/Catch2/src/catch2/catch_test_macros.hpp"

// using Catch::WithinAbs;

TEST_CASE("Demo Code", "[test]") {

  Demo demo;

  SECTION("doubles a number") { REQUIRE(demo.doubleANumber(2) == 4); }

  // SECTION("squares a float") {
  //   REQUIRE_THAT(demo.squareAFloat(2.54), WithinAbs(6.45f, .003f));
  // }
}
