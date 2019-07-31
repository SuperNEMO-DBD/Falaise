#include "catch.hpp"

#include "falaise/path.h"

TEST_CASE("Construction works", "") {
  falaise::path x{};
  REQUIRE(x == "");

  falaise::path x2{""};
  REQUIRE(x2 == "");

  falaise::path y{"foo"};
  REQUIRE(y == "foo");

  falaise::path z{"$HOME"};
  REQUIRE(z == std::string{getenv("HOME")});

  // Testing of "@mp" syntax requires a kernel
}

TEST_CASE("Copying works", "") {
  falaise::path ca = std::string("$HOME");
  REQUIRE(ca == std::string{getenv("HOME")});

  falaise::path cc{ca};
  REQUIRE(cc == ca);
}

TEST_CASE("Invalid path errors are reported", "") {
  REQUIRE_THROWS_AS(falaise::path{"$ABADENVVARTHATSHOULDNOTEXIST"}, falaise::invalid_path_error);
  REQUIRE_THROWS_AS(falaise::path{"/path/with spaces"}, falaise::invalid_path_error);
  REQUIRE_THROWS_AS(falaise::path c = {"/has/an/inval*dchar"}, falaise::invalid_path_error);
}

TEST_CASE("Implicit conversions work", "") {
  falaise::path p{"/foo/bar"};

  std::string s = p;
  REQUIRE(s == "/foo/bar");
}