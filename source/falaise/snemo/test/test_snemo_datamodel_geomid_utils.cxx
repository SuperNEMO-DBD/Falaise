// test_snemo_datamodels_geomid_utils.cxx

// Standard library
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <map>

// Bayeux:
#include <bayeux/datatools/service_manager.h>
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/geomtools/manager.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/datamodels/geomid_utils.h>
// #include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/locator_plugin.h>

void test0();
void test1();
void test2();

int main(/* int argc_, char ** argv_ */) {
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    
    test0();
    test1();
    test2();

  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}

void test1()
{
   
  // Service manager:
  datatools::properties serviceManagerConfig;
  serviceManagerConfig.store("logging", "debug");
  serviceManagerConfig.store_flag("force_initialization_at_load");
  std::vector<std::string> servicesConfigFiles = {
    "@falaise:snemo/demonstrator/geometry/5.0/GeometryServices.conf"};
  serviceManagerConfig.store("services.configuration_files", servicesConfigFiles);
  datatools::service_manager serviceManager;
  serviceManager.initialize(serviceManagerConfig);

  const geomtools::manager & geoManager
    = serviceManager.get<geomtools::geometry_service>("geometry").get_geom_manager();

  auto moduleNumber = 0u;
  
  datatools::properties mcaloLocConfig;
  snemo::geometry::calo_locator mcaloLoc(moduleNumber, geoManager, mcaloLocConfig);
  mcaloLoc.tree_dump(std::cerr, "main-calo locator:", "[debug] ");
  
  datatools::properties xcaloLocConfig;
  snemo::geometry::xcalo_locator xcaloLoc(moduleNumber, geoManager, xcaloLocConfig);
  xcaloLoc.tree_dump(std::cerr, "X-calo locator:", "[debug] ");

  datatools::properties gvetoLocConfig;
  snemo::geometry::gveto_locator gvetoLoc(moduleNumber, geoManager, gvetoLocConfig);
  gvetoLoc.tree_dump(std::cerr, "G-veto locator:", "[debug] ");

  std::map<geomtools::geom_id, int> om_map;
  std::map<int, geomtools::geom_id> inv_om_map;
  for (int side = 0; side < 2; side++) {
    for (int col = 0; col < 20; col++) {
      for (int row = 0; row < 13; row++) {
	geomtools::geom_id gid(1301, moduleNumber, side, col, row);
	DT_THROW_IF(not mcaloLoc.isCaloOM(gid), std::logic_error,
		    "Invalid MWALL calo OM gid " << gid << "!");
	auto num = snemo::datamodel::om_num(gid);
	// std::cout << num << ' ' << gid << '\n';
	om_map[gid] = num;
	inv_om_map[num] = gid;
      }
    }

    for (int wall = 0; wall < 2; wall++) {
      for (int col = 0; col < 2; col++) {
	for (int row = 0; row < 16; row++) {
	  geomtools::geom_id gid(1231, moduleNumber, side, wall, col, row);
	  DT_THROW_IF(not xcaloLoc.isCaloOM(gid), std::logic_error,
		      "Invalid XWALL calo OM gid " << gid << "!");
	  auto num = snemo::datamodel::om_num(gid);
	  // std::cout << num << ' ' << gid << '\n';
	  om_map[gid] = num;
	  inv_om_map[num] = gid;
	}
      }
    }
   
    for (int wall = 0; wall < 2; wall++) {
      for (int row = 0; row < 16; row++) {
	geomtools::geom_id gid(1251, moduleNumber, side, wall, row);
	DT_THROW_IF(not gvetoLoc.isCaloOM(gid), std::logic_error,
		    "Invalid GVETO calo OM gid " << gid << "!");
	auto num = snemo::datamodel::om_num(gid);
	om_map[gid] = num;
	inv_om_map[num] = gid;
      }
    }
     
  }

  std::cout << "\nOM map:\n";
  for (const auto & omid : inv_om_map) {
    std::cout << omid.first << ' ' << omid.second;
    auto checkOMGid = snemo::datamodel::om_gid(omid.first, false);
    std::cout << ' ' << checkOMGid;
    auto checkBackBlockGid = snemo::datamodel::om_gid(omid.first, true, true);
    std::cout << ' ' << checkBackBlockGid;
    std::cout << '\n';
  }
  
  return;
}

void test0()
{
 
  std::cout << "\nOM map:\n";
  for (const auto & gid : snemo::datamodel::om_map()) {
    std::cout << gid.first << ' ' << gid.second;
    if (not snemo::datamodel::is_reference_om(gid.second)) {
      auto checkGid = snemo::datamodel::om_gid(gid.first, false, false);
      std::cout << ' ' << checkGid;
    }
    std::cout << '\n';
  }
 
  return;
}

void test2()
{
 
  std::cout << "\nTracker cell map:\n";
  for (const auto & ggid : snemo::datamodel::gg_map()) {
    std::cout << ggid.first << ' ' << ggid.second;
    auto checkGid = snemo::datamodel::gg_gid(ggid.first, false);
    std::cout << ' ' << checkGid;
    auto checkCoreGid = snemo::datamodel::gg_gid(ggid.first, true);
    std::cout << ' ' << checkCoreGid;
    std::cout << '\n';
  }
 
  return;
}
