// Catch
#include "catch.hpp"

#include "falaise/snemo/services/service_handle.h"
#include "falaise/snemo/services/geometry.h"
#include "falaise/snemo/services/histogram.h"

#include "bayeux/datatools/service_manager.h"
#include "bayeux/datatools/multi_properties.h"


TEST_CASE("Default construction works", "") {
  snemo::service_handle<snemo::geometry_svc> x;
  REQUIRE_THROWS_AS( x->is_debug(), snemo::bad_service_access);
}

TEST_CASE("Construction from bad services fails", "") {
  datatools::service_manager dummyServices{};

  SECTION("non-existant service throws") {
    REQUIRE_THROWS_AS( snemo::service_handle<snemo::geometry_svc> x{dummyServices}, snemo::missing_service_error );
  }

  SECTION("service label/type mismatch throws") {
    datatools::multi_properties config;
    config.add_section("geometry", "dpp::histogram_service");
    dummyServices.load(config);
    REQUIRE_THROWS_AS( snemo::service_handle<snemo::geometry_svc> x{dummyServices}, snemo::bad_service_type );
  }
}

TEST_CASE("Construction from good services works", "") {
  datatools::service_manager dummyServices{};
  datatools::multi_properties config;
  config.add_section("geometry", "geomtools::geometry_service")
      .store_path("manager.configuration_file", "@falaise:snemo/demonstrator/geometry/GeometryManager.conf");
  config.add_section("histogram", "dpp::histogram_service");

  dummyServices.load(config);
  dummyServices.initialize();

  SECTION("manager-holder service works") {
    snemo::service_handle<snemo::geometry_svc> x{dummyServices};
    REQUIRE_NOTHROW( x->is_debug() );
  }

  SECTION("direct service works") {
    snemo::service_handle<snemo::histogram> x{dummyServices};
    REQUIRE_NOTHROW( x->has_output_files() );
  }
}


