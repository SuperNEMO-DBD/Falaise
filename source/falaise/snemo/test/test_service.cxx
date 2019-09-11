// Catch
#include "catch.hpp"

#include "falaise/snemo/services/geometry.h"
#include "falaise/snemo/services/histogram.h"
#include "falaise/snemo/services/hello_world.h"
#include "falaise/snemo/services/service_handle.h"

#include "bayeux/datatools/multi_properties.h"
#include "bayeux/datatools/service_manager.h"

TEST_CASE("Default construction works", "") {
  snemo::service_handle<snemo::geometry_svc> x;
  REQUIRE_THROWS_AS(x->is_debug(), snemo::bad_service_access);
}

TEST_CASE("Construction from bad services fails", "") {
  datatools::service_manager dummyServices{};

  SECTION("non-existant service throws") {
    REQUIRE_THROWS_AS((snemo::service_handle<snemo::geometry_svc>{dummyServices}),
                      snemo::missing_service_error);
  }

  SECTION("service label/type mismatch throws") {
    datatools::multi_properties config;
    config.add_section("geometry", "dpp::histogram_service");
    dummyServices.load(config);
    REQUIRE_THROWS_AS(snemo::service_handle<snemo::geometry_svc>{dummyServices},
                      snemo::bad_service_type);
  }
}

TEST_CASE("Construction from good services works", "") {
  datatools::service_manager dummyServices{};
  datatools::multi_properties config;
  config.add_section("geometry", "geomtools::geometry_service")
      .store_path("manager.configuration_file",
                  "@falaise:snemo/demonstrator/geometry/GeometryManager.conf");
  config.add_section("histogram", "dpp::histogram_service");
  config.add_section("hello_world_svc", "snemo::hello_world_svc");

  dummyServices.load(config);
  dummyServices.initialize();

  SECTION("manager-holder service works") {
    snemo::service_handle<snemo::geometry_svc> x{dummyServices};
    REQUIRE_NOTHROW(x->is_debug());
  }

  SECTION("direct service works") {
    snemo::service_handle<snemo::histogram> x{dummyServices};
    REQUIRE_NOTHROW(x->has_output_files());
  }

  SECTION("builtin service works") {
    snemo::service_handle<snemo::hello_world_svc> x{dummyServices};
    REQUIRE_NOTHROW(x->is_initialized());
    REQUIRE(x->say_hello("Falaise") == std::string("Hello Falaise"));
  }
}
