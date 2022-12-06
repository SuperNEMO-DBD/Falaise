// Catch
#include "catch.hpp"

#include "falaise/snemo/services/geometry.h"
#include "falaise/snemo/services/service_handle.h"
#include "falaise/snemo/geometry/config.h"
#include "falaise/snemo/geometry/gveto_locator.h"
#include "falaise/snemo/geometry/locator_plugin.h"
#include "falaise/snemo/geometry/locator_helpers.h"

#include "bayeux/datatools/multi_properties.h"
#include "bayeux/datatools/service_manager.h"


TEST_CASE("Exercise Issue 199", "") {
  datatools::service_manager dummyServices{};
  datatools::multi_properties config;              
  config.set_key_label("name");
  config.set_meta_label("type");
  // config.add_section("geometry", "geomtools::geometry_service")
  //     .store_path("manager.configuration_file",
  //                 "@falaise:snemo/demonstrator/geometry/GeometryManager.conf");
  config.add_section("geometry", "geomtools::geometry_service")
      .store_path("manager.configuration_file",
                  snemo::geometry::default_geometry_tag());
  dummyServices.load(config);
  dummyServices.initialize();
  snemo::service_handle<snemo::geometry_svc> gs{dummyServices};

  // Get the gveto locator
  const snemo::geometry::locator_plugin* lp = snemo::geometry::getSNemoLocator(*(gs.operator->()),"locators_driver");
  const snemo::geometry::gveto_locator& gl = lp->gvetoLocator();

  // Check sides/walls/columns
  // Only one module
  const uint32_t NModule = gl.getModuleNumber();
  const uint32_t NSides = gl.numberOfSides();
  const uint32_t NWalls = gl.numberOfWalls();

  // These are pure sanity checks, albeit hardcoded
  REQUIRE(NModule == 0);
  REQUIRE(NSides == 2);
  REQUIRE(NWalls == 2);

  // We can count columns over side/wall - again expectation is hardcoded
  for(uint32_t s=0; s < NSides; ++s) {
    for(uint32_t w=0; w < NWalls; ++w) {
      REQUIRE(gl.numberOfColumns(s,w) == 16);
    }
  }

  // Expected GIDs are [category:module:side:wall:column]
  // Category is "1253" (from resources/snemo/demonstrator/geometry/GeomIDMaps/tracker_categories.lis)
  // Module is fixed here at "0"
  // Then have 2x2x16 range of allowed GIDs (so 64 blocks)
  for(uint32_t s=0; s < NSides; ++s) {
    for(uint32_t w=0; w < NWalls; ++w) {
      for(uint32_t c=0; c < 16; ++c) {
        REQUIRE(gl.isValidAddress(s,w,c));
        // This should exercise Issue 199
        REQUIRE_NOTHROW(gl.getBlockPosition(s,w,c));
      }
    }
  }
}
