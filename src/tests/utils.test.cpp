#include "../volumegate/utils.h"
#include "../lib/Catch2/src/catch2/catch_test_macros.hpp"
#include "../lib/Catch2/src/catch2/matchers/catch_matchers_all.hpp"

// using Catch::WithinAbs;
using Catch::Matchers::WithinAbs;

// int max_samplet = 32767;
// int min_samplet = -32768;
TEST_CASE("sample to dbfs", "[test]") {

  // Demo demo;

  SECTION("does nothing") { REQUIRE(0 == 0); }
  SECTION("returns 0.0f for full sample") {
    REQUIRE(sample_to_dbfs(max_sample) == 0);
    REQUIRE(sample_to_dbfs(min_sample) == 0);
    REQUIRE(sample_to_dbfs(10000) == -10.308732986f);
    REQUIRE_THAT(sample_to_dbfs(-10000), WithinAbs(-10.308732986f, .003f));
    REQUIRE(sample_to_dbfs(0) == -160.0f);
  }

  // SECTION("squares a float") {
  //   REQUIRE_THAT(demo.squareAFloat(2.54), WithinAbs(6.45f, .003f));
  // }
}
