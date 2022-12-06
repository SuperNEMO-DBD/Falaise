// test_snemo_services_tracker_cell_status_service.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/services/tracker_cell_status_service.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/falaise.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::service::tracker_cell_status_service'!" << std::endl;
    test1();
    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
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
  std::clog << "\nTest 1:\n";
 
  geomtools::manager geoMgr;
  std::string geoMgrConfigFile("@falaise:snemo/demonstrator/geometry/5.0/GeometryManager.conf");
  datatools::fetch_path_with_env(geoMgrConfigFile);
  datatools::properties geoMgrConfig;
  geoMgrConfig.read_configuration(geoMgrConfigFile);
  geoMgr.initialize(geoMgrConfig);
   
  namespace snrc = snemo::rc;
  snemo::tracker_cell_status_service cellService;
  datatools::properties cellServiceConfig;
  cellServiceConfig.store("mode", "files");
  std::vector<std::string> cellStatusMapFiles;
  cellStatusMapFiles = {"${FALAISE_SNEMO_TESTING_DIR}/config/test-tracker_dead_cells-1.conf",
                        "${FALAISE_SNEMO_TESTING_DIR}/config/test-tracker_cell_status_map.conf"};
  cellServiceConfig.store("files.cell_maps", cellStatusMapFiles);
  cellService.set_geometry_manager(geoMgr);
  cellService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  cellService.initialize_standalone(cellServiceConfig);
  cellService.print_tree(std::clog);

  namespace snt = snemo::time;
  {
    std::ofstream fout("test-tracker_dead_cells-draw-period1.data");
    snt::time_point mapTime = snt::time_point(snt::date(2022, 6, 13),
                                              snt::hours(0) + snt::minutes(0));
    cellService.make_cell_status_map(fout, mapTime);
    fout.close();
  }

  {
    std::ofstream fout("test-tracker_dead_cells-draw-period2.data");
    snt::time_point mapTime = snt::time_point(snt::date(2022, 6, 25),
                                              snt::hours(2) + snt::minutes(12));
    cellService.make_cell_status_map(fout, mapTime);
    fout.close();
  }
  
  cellService.reset();
  geoMgr.reset();
  return;
}
