#include "catch.hpp"
#include "mem_tracker/tracker.hpp"

TEST_CASE("tracking allocation default aligned", "[allocation][default aligned]") {
  SECTION("tracking allocation of an int") {
    mem_tracker::tracker t{};
    REQUIRE(std::begin(t) == std::end(t));
  }
}