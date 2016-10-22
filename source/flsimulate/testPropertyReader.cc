// Catch
#include "catch.hpp"

#include "PropertyReader.h"

namespace flp = falaise::Properties;

TEST_CASE("PropertyReader works", "") {
  datatools::properties test;
  test.store("foo", 1);
  test.store("bar", 3.14);
  test.store("baz", true);
  std::vector<int> count {1,2,3,4};
  test.store("count", count);
  std::vector<int> singular {42};
  test.store("singular", singular);

  SECTION("Integer extraction") {
    REQUIRE( flp::getRequiredValue<int>(test,"foo") == 1 );
    REQUIRE_THROWS_AS( flp::getRequiredValue<double>(test, "foo"), flp::WrongType );

    REQUIRE( flp::getValueOrDefault<int>(test,"foo",42) == 1 );
    REQUIRE( flp::getValueOrDefault<int>(test,"ICouldBePresentButAmNot",42) == 42 );
  }

  SECTION("Real extraction") {
    REQUIRE( flp::getRequiredValue<double>(test,"bar") == Approx(3.14) );
    REQUIRE_THROWS_AS( flp::getRequiredValue<int>(test,"bar"), flp::WrongType );
  }

  SECTION("Boolean extraction") {
    REQUIRE( flp::getRequiredValue<bool>(test,"baz") );
    REQUIRE_FALSE( flp::getValueOrDefault<bool>(test,"ICouldBePresentButAmNot",false) );
    REQUIRE_THROWS_AS( flp::getRequiredValue<bool>(test,"foo"), flp::WrongType );
  }

  SECTION("Vector extraction") {
    auto vec = flp::getRequiredValue<std::vector<int> >(test,"count");
    REQUIRE( vec == count );

    // A single element vector is still a vector...
    REQUIRE_NOTHROW( flp::getRequiredValue<std::vector<int> >(test,"singular") );

    // ... and is not convertible to a single element
    REQUIRE_THROWS_AS( flp::getRequiredValue<int>(test,"singular"), flp::WrongType );
  }

  SECTION("Failure conditions") {
    // Unsupported types should be a compile-time error
    REQUIRE_THROWS_AS( flp::getRequiredValue<double>(test, "IMustBePresentButAmNot"), flp::MissingKey );
  }
}


