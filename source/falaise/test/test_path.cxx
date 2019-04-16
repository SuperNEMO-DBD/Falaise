#include "catch.hpp"

#include "falaise/config/path.h"

TEST_CASE("Construction works", "") {
  falaise::config::path x{};
  REQUIRE(x == "");

  falaise::config::path y{"foo"};
  REQUIRE(y == "foo");

  falaise::config::path z{"$HOME"};
  REQUIRE(z == std::string{getenv("HOME")});

  // Testing of "@mp" syntax requires a kernel
}

TEST_CASE("Copying works", "") {
  falaise::config::path ca = std::string("$HOME");
  REQUIRE(ca == std::string{getenv("HOME")});

  falaise::config::path cc{ca};
  REQUIRE(cc == ca);
}

TEST_CASE("Invalid path errors are reported", "") {
  REQUIRE_THROWS_AS(falaise::config::path{"$ABADENVVARTHATSHOULDNOTEXIST"}, falaise::config::invalid_path_error);
  REQUIRE_THROWS_AS(falaise::config::path{"/path/with spaces"}, falaise::config::invalid_path_error);
  REQUIRE_THROWS_AS(falaise::config::path c = {"/has/an/inval*dchar"}, falaise::config::invalid_path_error);
}

TEST_CASE("Implicit conversions work", "") {
  falaise::config::path p{"/foo/bar"};

  std::string s = p;
  REQUIRE(s == "/foo/bar");
}