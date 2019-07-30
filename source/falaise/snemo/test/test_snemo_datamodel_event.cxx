// test_event.cxx
#include <falaise/snemo/datamodels/event.h>
#include "catch.hpp"

#include <falaise/snemo/datamodels/timestamp.h>

TEST_CASE("Event construction works", "[falaise][datamodel]") {
  // Default
  snemo::datamodel::event_record e{"foo", "bar"};

  REQUIRE(e.has_name());
  REQUIRE(e.get_description() == "bar");
}

TEST_CASE("getOrAddToEvent free function works", "[falaise][datamodel]") {
   snemo::datamodel::event_record e;
   using timestamp = snemo::datamodel::timestamp;

   SECTION("Get on an empty event creates 1 entry") {
     REQUIRE_NOTHROW(auto& t0 = snemo::datamodel::getOrAddToEvent<timestamp>("first", e));
     REQUIRE(e.size() == 1);
     REQUIRE(e.has("first"));
     REQUIRE(e.is_a<timestamp>("first"));
   }

   SECTION("Get on a prefilled event gets the same object") {
     auto& t0 = snemo::datamodel::getOrAddToEvent<timestamp>("first", e);
     REQUIRE_NOTHROW(auto& t1 = snemo::datamodel::getOrAddToEvent<timestamp>("first", e)); 
     REQUIRE(e.size() == 1);
     auto& t1 = snemo::datamodel::getOrAddToEvent<timestamp>("first", e); 
     REQUIRE( &t0 == &t1 );
   }

   SECTION("Getting on different type throws bad_things_cast") {
      auto& t0 = snemo::datamodel::getOrAddToEvent<timestamp>("first", e); 

      REQUIRE_THROWS_AS(auto& t1 = snemo::datamodel::getOrAddToEvent<datatools::things>("first", e), datatools::bad_things_cast);
   }
}