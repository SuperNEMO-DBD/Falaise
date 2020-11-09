// Catch
#include "catch.hpp"

#include "falaise/snemo/services/service_handle.h"
#include "falaise/snemo/services/dead_cells.h"

#include "bayeux/datatools/multi_properties.h"
#include "bayeux/datatools/service_manager.h"

TEST_CASE("Construction from good services works", "") {
  datatools::service_manager dummyServices{};
  datatools::multi_properties config;
  config.add_section("dead_cells_svc", "snemo::dead_cells_svc");

  dummyServices.load(config);
  dummyServices.initialize();

  SECTION("dead cells service works") {
    snemo::service_handle<snemo::dead_cells_svc> x{dummyServices};
    REQUIRE_NOTHROW(x->is_initialized());
    REQUIRE_NOTHROW(x->LoadCells("../../share/Falaise-4.0.3/resources/snemo/demonstrator/test_dead_cells.txt"));
    REQUIRE(x->LoadCells("../../share/Falaise-4.0.3/resources/snemo/demonstrator/test_dead_cells.txt") == 0);
    REQUIRE(x->CellStatus(1,1,1,100) == snemo::cell_status::good);
    REQUIRE(x->CellStatus(0,2,4,100) == snemo::cell_status::dead);
    REQUIRE(x->CellStatus(0,3,5,100) == snemo::cell_status::cathode_ground_top);
    REQUIRE(x->CellStatus(1,4,6,100) == snemo::cell_status::cathode_ground_bottom);
    REQUIRE(x->CellStatus(1,5,7,100) == snemo::cell_status::cathode_ground_both);
    REQUIRE(x->CellStatus(0,6,8,100) == snemo::cell_status::cathode_cathode);
    REQUIRE(x->CellStatus(0,7,9,100) == snemo::cell_status::other);
    REQUIRE(x->isABadCell(0,2,4,100) == 1);    
    REQUIRE(x->isAGoodCell(0,2,4,100) == 0);    
    REQUIRE(x->isADeadCell(0,2,4,100) == 1);    
    snemo::cell_id cid(0,2,4);
    REQUIRE(cid.GetSide() == 0);
    REQUIRE(cid.GetLayer() == 2);
    REQUIRE(cid.GetColumn() == 4);
    //REQUIRE(cid.GetStatus() == snemo::cell_status::dead);
    REQUIRE(x->CellStatus(cid,100) == snemo::cell_status::dead);
    REQUIRE(x->isABadCell(cid,100) == 1);
    REQUIRE(x->isAGoodCell(cid,100) == 0);
    REQUIRE(x->isADeadCell(cid,100) == 1);    
   }



}
