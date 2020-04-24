// Catch
#include "catch.hpp"

#include "falaise/snemo/datamodels/event.h"

TEST_CASE("Adding product at existing key throws", "") {
  snedm::event_record e;

  REQUIRE_NOTHROW(snedm::addToEvent<snedm::event_record>("first", e));
  REQUIRE_THROWS_AS(snedm::addToEvent<snedm::event_record>("first", e),
                    snedm::existing_product_key);
}