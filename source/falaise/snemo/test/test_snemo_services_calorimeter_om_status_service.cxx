// test_snemo_services_calorimeter_om_status_service.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/services/calorimeter_om_status_service.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/falaise.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::service::calorimeter_om_status_service'!" << std::endl;
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
  std::string geoMgrConfigFile("@falaise:snemo/demonstrator/geometry/GeometryManager.conf");
  datatools::fetch_path_with_env(geoMgrConfigFile);
  datatools::properties geoMgrConfig;
  geoMgrConfig.read_configuration(geoMgrConfigFile);
  geoMgr.initialize(geoMgrConfig);
   
  namespace snrc = snemo::rc;
  snemo::calorimeter_om_status_service omService;
  datatools::properties omServiceConfig;
  omServiceConfig.store("mode", "files");
  std::vector<std::string> omStatusMapFiles;
  omStatusMapFiles = {"${FALAISE_SNEMO_TESTING_DIR}/config/test-calorimeter_off_om-1.conf"};
  omServiceConfig.store("files.om_maps", omStatusMapFiles);
  omService.set_geometry_manager(geoMgr);
  omService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  omService.initialize_standalone(omServiceConfig);

  // const snemo::rc::calorimeter_om_status_history & omStatusHistory =
  //   omService.get_om_history(gid);
  // namespace snt = snemo::time;
  // {
  //   std::ofstream fout("test-tracker_dead_cells-draw-period1.data");
  //   snt::time_point mapTime = snt::time_point(snt::date(2022, 6, 13),
  //                                             snt::hours(0) + snt::minutes(0));
  //   cellService.make_cell_status_map(fout, mapTime);
  //   fout.close();
  // }

  // {
  //   std::ofstream fout("test-tracker_dead_cells-draw-period2.data");
  //   snt::time_point mapTime = snt::time_point(snt::date(2022, 6, 25),
  //                                             snt::hours(2) + snt::minutes(12));
  //   cellService.make_cell_status_map(fout, mapTime);
  //   fout.close();
  // }
  
  omService.reset();
  geoMgr.reset();
  return;
}
