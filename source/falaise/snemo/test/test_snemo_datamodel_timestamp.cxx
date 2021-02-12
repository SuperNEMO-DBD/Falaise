// test_timestamp.cxx
#include <falaise/snemo/datamodels/timestamp.h>
#include "catch.hpp"

TEST_CASE("Timestamps are correctly constructed", "[falaise][datamodel]") {
  // Default
  snemo::datamodel::timestamp t1;

  REQUIRE_FALSE(t1.is_valid());

  // With times
  snemo::datamodel::timestamp t2{12345, 678910};
  REQUIRE(t2.get_seconds() == 12345);
  REQUIRE(t2.get_picoseconds() == 678910);
  REQUIRE(t2.is_valid());
}


TEST_CASE("Timestamps can be copied and compared", "[falaise][datamodel]") {
  snemo::datamodel::timestamp t1{12345, 678910};
  const snemo::datamodel::timestamp& t1_copy(t1);

  snemo::datamodel::timestamp t2_sec{12346, 678910};
  snemo::datamodel::timestamp t2_pico{12345, 678911};

  snemo::datamodel::timestamp tInvalid;

  REQUIRE(t1 == t1_copy);
  REQUIRE(t1 < t2_sec);
  REQUIRE(t2_pico > t1);

  REQUIRE_THROWS(t1 == tInvalid);
}

TEST_CASE("Timestamps can be read from/written to strings", "[falaise][datamodel]") {
  snemo::datamodel::timestamp tTest;
  std::string sTest;

  snemo::datamodel::timestamp t1{12345, 678910};
  std::string t1String{"[12345:678910]"};

  snemo::datamodel::timestamp tInvalid;
  std::string tStringInvalid{"[?:?]"};

  std::string tStringValue{"[54321:987654321]"};
  snemo::datamodel::timestamp tStringValueTime{54321, 987654321};

  SECTION("Valid timestamp from std::string works") {
    tTest.from_string(tStringValue);
    REQUIRE(tTest == tStringValueTime);
  }

  SECTION("Valid timestamp to std::string works") {
    t1.to_string(sTest);
    REQUIRE(sTest == t1String);
  }

  SECTION("Invalid timestamp to/from std::string works") {
    tTest.from_string(tStringInvalid);
    REQUIRE_FALSE(tTest.is_valid());
    tTest.to_string(sTest);
    REQUIRE(sTest == tStringInvalid);
  }
}
