// Catch
#include "catch.hpp"

#include "falaise/config/property_reader.h"

namespace flp = falaise::config;

TEST_CASE("property_reader scalars Work", "") {
  datatools::properties test;
  test.store("foo", 1);
  test.store("bar", 3.14);
  test.store("baz", true);
  test.store("flatstring", "foobar");

  SECTION("Integer extraction") {
    REQUIRE(flp::getRequiredValue<int>(test, "foo") == 1);
    REQUIRE_THROWS_AS(flp::getRequiredValue<double>(test, "foo"), flp::WrongType&);

    REQUIRE(flp::getValueOrDefault<int>(test, "foo", 42) == 1);
    REQUIRE(flp::getValueOrDefault<int>(test, "ICouldBePresentButAmNot", 42) == 42);
  }

  SECTION("Real extraction") {
    REQUIRE(flp::getRequiredValue<double>(test, "bar") == Approx(3.14));
    REQUIRE_THROWS_AS(flp::getRequiredValue<int>(test, "bar"), flp::WrongType&);
  }

  SECTION("Boolean extraction") {
    REQUIRE(flp::getRequiredValue<bool>(test, "baz"));
    REQUIRE_FALSE(flp::getValueOrDefault<bool>(test, "ICouldBePresentButAmNot", false));
    REQUIRE_THROWS_AS(flp::getRequiredValue<bool>(test, "foo"), flp::WrongType&);
  }

  SECTION("String extraction") {
    REQUIRE(flp::getRequiredValue<std::string>(test, "flatstring") == "foobar");
    REQUIRE(flp::getValueOrDefault<std::string>(test, "ICouldBePresentButAmNot", "adefault") ==
            "adefault");
  }

  SECTION("Failure conditions") {
    // Unsupported types should be a compile-time error
    REQUIRE_THROWS_AS(flp::getRequiredValue<double>(test, "IMustBePresentButAmNot"),
                      flp::MissingKey&);
  }
}

TEST_CASE("property_reader vector extraction", "") {
  datatools::properties test;
  std::vector<int> count{1, 2, 3, 4};
  test.store("count", count);
  std::vector<int> singular{42};
  test.store("singular", singular);

  std::vector<double> values{1.2, 3.4, 5.6, 7.8};
  test.store("values", values);

  std::vector<bool> flags{true, false, false, true, true};
  test.store("flags", flags);

  std::vector<std::string> names{"foo", "bar", "baz"};
  test.store("names", names);

  SECTION("Integer Vector extraction") {
    auto vec = flp::getRequiredValue<std::vector<int> >(test, "count");
    REQUIRE(vec == count);

    // A single element vector is still a vector...
    REQUIRE_NOTHROW(flp::getRequiredValue<std::vector<int> >(test, "singular"));

    // ... and is not convertible to a single element
    REQUIRE_THROWS_AS(flp::getRequiredValue<int>(test, "singular"), flp::WrongType&);

    // Defaults should work in place
    // NB: test composition does outside macro due to seeming limitation of not
    // being allowed to use brace initialization inside a macro statement.
    std::vector<int> def{4, 5, 6};
    auto result =
        flp::getValueOrDefault<std::vector<int> >(test, "ICouldBePresentButAmNot", {4, 5, 6});
    REQUIRE(result == def);
  }

  SECTION("Double Vector extraction") {
    auto vec = flp::getRequiredValue<std::vector<double> >(test, "values");
    REQUIRE(vec == values);
  }

  SECTION("Boolean Vector extraction") {
    REQUIRE(flp::getRequiredValue<std::vector<bool> >(test, "flags") == flags);
  }

  SECTION("String Vector extraction") {
    REQUIRE(flp::getRequiredValue<std::vector<std::string> >(test, "names") == names);
  }
}
