#include "catch.hpp"

#include "falaise/bounded_int.h"

TEST_CASE("Raw bounded construction", "") {
  falaise::bounded_int<-1, 42> x{};
  REQUIRE(x == -1);
  REQUIRE(x.min() == -1);
  REQUIRE(x.max() == 42);
}

// To be refactored once we find a good method for
// testing compile time constraints
// TEST_CASE("Compile time bounds checking", "") {
//  // Should not be able to compile these!
//  falaise::bounded_int<2,1> x{};
//  falaise::bounded_int<5,5> y{};
//}

TEST_CASE("Out of bounds construction", "") {
  using address = falaise::bounded_int<0, 119>;

  // Must be able to construct at bounds
  REQUIRE_NOTHROW(address{0});
  REQUIRE_NOTHROW(address{119});

  // Out of bounds must throw
  REQUIRE_THROWS_AS(address{-1}, std::out_of_range);
  REQUIRE_THROWS_AS(address{120}, std::out_of_range);
}

TEST_CASE("Assignment operators work", "") {
  using address = falaise::bounded_int<-42, 119>;

  address x;

  // Assignment must work
  x = 3;
  REQUIRE(x == 3);

  // Assignment on bounds must work
  REQUIRE_NOTHROW(x = -42);
  REQUIRE_NOTHROW(x = 119);

  // Out of bounds assignment must throw
  REQUIRE_THROWS_AS(x = -43, std::out_of_range);
  REQUIRE_THROWS_AS(x = -120, std::out_of_range);
}

TEST_CASE("Assignments from/to other builtin integrals", "") {
  // NB: Relies on implicit conversions, which clang-tidy and
  // so on may warn about. In that case, likely just need
  // non-explicit constructors. Conversion to (operator T())
  // other integrals would need to check that bounds fit inside
  // limits
  falaise::bounded_int<4,10> x;
  size_t y = 5;
  uint8_t z = 2;

  REQUIRE_NOTHROW(x = y);
  REQUIRE_THROWS_AS(x = z, std::out_of_range);
}