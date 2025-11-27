// test_snemo_services_calorimeter_om_status_service.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Bayeux:
#include <bayeux/datatools/service_manager.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/geometry/config.h>
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/services/calorimeter_om_status_service.h>

void test1();
void test2();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::service::calorimeter_om_status_service'!" << std::endl;
    // test1();
    test2();
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
  std::string geoMgrConfigFile(snemo::geometry::default_geometry_tag());
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
  omService.print_tree(std::clog);

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

void test2()
{
  std::clog << "\nTest 2:\n";
 
  std::string dbServiceMgrConfigPath = "@falaise:snemo/demonstrator/db/0.1/services/DbService.conf";
  datatools::fetch_path_with_env(dbServiceMgrConfigPath);
  datatools::multi_properties dbServiceMgrConfig("name", "type");
  dbServiceMgrConfig.read(dbServiceMgrConfigPath);
 
  std::string geomServiceMgrConfigPath = "@falaise:snemo/demonstrator/geometry/5.0/GeometryServices.conf";
  datatools::fetch_path_with_env(geomServiceMgrConfigPath);
  datatools::multi_properties geomServiceMgrConfig("name", "type");
  geomServiceMgrConfig.read(geomServiceMgrConfigPath);

  
  datatools::service_manager serviceMgr;
  serviceMgr.set_logging_priority(datatools::logger::PRIO_DEBUG);
  serviceMgr.load(dbServiceMgrConfig);
  serviceMgr.load(geomServiceMgrConfig);
  serviceMgr.initialize();
  
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;
  datatools::properties omServiceConfig;
  omServiceConfig.store("mode", "db");
  omServiceConfig.store("geometry_label", snemo::service_info::geometryServiceName());
  omServiceConfig.store("db_label", snemo::service_info::dbServiceName());
  omServiceConfig.store("db.table_name", "_test_om_status");

  snemo::calorimeter_om_status_service omService;
  omService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  omService.initialize(omServiceConfig,
		       const_cast<datatools::service_dict_type&>(serviceMgr.get_local_services()));
  omService.print_tree(std::clog);
  
  {
    geomtools::geom_id omGid(1301, 0, 1, 13, 12);
    snt::time_point mapTime = snt::time_point(snt::date(2022, 6, 13),
                                              snt::hours(0) + snt::minutes(0));
    std::uint32_t omStatus = omService.get_om_status(omGid, mapTime);
    std::cout << "OM status = " << omStatus << '\n';
  }

  omService.reset();
  return;
}
