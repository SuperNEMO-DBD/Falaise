// test_snemo_services_tracker_cell_status_service.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/geometry/config.h>
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/services/tracker_cell_status_service.h>
#include <falaise/snemo/datamodels/geomid_utils.h>

void test1();
void test2();
void test3();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::service::tracker_cell_status_service'!" << std::endl;
    // test1();
    // test2();
    test3();
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
  cellServiceConfig.store("file.format", "csv-1");
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

void test2()
{
  std::clog << "\nTest 2:\n";
 
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
  cellServiceConfig.store("files.map_format", "csv-2");
  std::vector<std::string> cellStatusMapFiles;
  cellStatusMapFiles = {"${FALAISE_SNEMO_TESTING_DIR}/config/test-tracker_dead_cells-1-v2.conf",
                        "${FALAISE_SNEMO_TESTING_DIR}/config/test-tracker_cell_status_map-v2.conf"};
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

void test3()
{
  std::clog << "\nTest 3:\n";
  datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
  // verbosity = datatools::logger::PRIO_DEBUG;
  std::string dbServiceMgrConfigPath = "@falaise:snemo/demonstrator/db/0.1/services/DbService.conf";
  datatools::fetch_path_with_env(dbServiceMgrConfigPath);
  datatools::multi_properties dbServiceMgrConfig("name", "type");
  dbServiceMgrConfig.read(dbServiceMgrConfigPath);
 
  std::string geomServiceMgrConfigPath = "@falaise:snemo/demonstrator/geometry/6.0/GeometryServices.conf";
  datatools::fetch_path_with_env(geomServiceMgrConfigPath);
  datatools::multi_properties geomServiceMgrConfig("name", "type");
  geomServiceMgrConfig.read(geomServiceMgrConfigPath);
  
  datatools::service_manager serviceMgr;
  serviceMgr.set_logging_priority(verbosity);
  serviceMgr.load(dbServiceMgrConfig);
  serviceMgr.load(geomServiceMgrConfig);
  serviceMgr.initialize();
  
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;
  datatools::properties ggServiceConfig;
  ggServiceConfig.store("mode", "db");
  ggServiceConfig.store("geometry_label", snemo::service_info::geometryServiceName());
  ggServiceConfig.store("db_label", snemo::service_info::dbServiceName());
  ggServiceConfig.store("db.table_name", "GG_Status_Change_Events");

  snemo::tracker_cell_status_service ggService;
  ggService.set_logging_priority(verbosity);
  ggService.initialize(ggServiceConfig,
		       const_cast<datatools::service_dict_type&>(serviceMgr.get_local_services()));
  if (datatools::logger::is_debug(verbosity)) {
    boost::property_tree::ptree printOpts;
    printOpts.put("indent", "[debug] ");
    ggService.print_tree(std::clog, printOpts);
  }
   
  {
    geomtools::geom_id ggGid(1203, 0, 0, 0, 0);
    snt::time_point mapTime = snt::time_point(snt::date(2025, 4, 10),
                                              snt::hours(19) + snt::minutes(0));
    std::uint32_t ggStatus = ggService.get_cell_status(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}' << " -> status = " << ggStatus
	      << " [" << snrc::tracker_cell_status::status_to_string(ggStatus) << ']' << '\n';
  }
 
  {
    geomtools::geom_id ggGid(1203, 0, 0, 0, 1);
    snt::time_point mapTime = snt::time_point(snt::date(2025, 4, 10),
                                              snt::hours(19) + snt::minutes(0));
    std::uint32_t ggStatus = ggService.get_cell_status(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}' << " -> status = " << ggStatus
	      << " [" << snrc::tracker_cell_status::status_to_string(ggStatus) << ']' << '\n';
  }
   
  {
    geomtools::geom_id ggGid(1203, 0, 0, 0, 109);
    snt::time_point mapTime = snt::time_point(snt::date(2025, 4, 10),
                                              snt::hours(19) + snt::minutes(0));
    std::uint32_t ggStatus = ggService.get_cell_status(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}' << " -> status = " << ggStatus
	      << " [" << snrc::tracker_cell_status::status_to_string(ggStatus) << ']' << '\n';
  }
 
  {
    geomtools::geom_id ggGid(1203, 0, 1, 0, 109);
    snt::time_point mapTime = snt::time_point(snt::date(2025, 4, 10),
                                              snt::hours(19) + snt::minutes(0));
    std::uint32_t ggStatus = ggService.get_cell_status(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}' << " -> status = " << ggStatus
	      << " [" << snrc::tracker_cell_status::status_to_string(ggStatus) << ']' << '\n';
  }
  
  {
    geomtools::geom_id ggGid(1203, 0, 0, 0, 109);
    snt::time_point mapTime = snt::time_point(snt::date(2025, 7, 8),
                                              snt::hours(19) + snt::minutes(0));
    std::uint32_t ggStatus = ggService.get_cell_status(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}' << " -> status = " << ggStatus
	      << " [" << snrc::tracker_cell_status::status_to_string(ggStatus) << ']' << '\n';
  }
 
  {
    geomtools::geom_id ggGid(1203, 0, 1, 0, 109);
    snt::time_point mapTime = snt::time_point(snt::date(2025, 7, 8),
                                              snt::hours(19) + snt::minutes(0));
    std::uint32_t ggStatus = ggService.get_cell_status(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}' << " -> status = " << ggStatus
	      << " [" << snrc::tracker_cell_status::status_to_string(ggStatus) << ']' << '\n';
  }

  {
    geomtools::geom_id ggGid(1203, 0, 1, 5, 66); // #1616 @ phase 0
    snt::time_point mapTime = snt::time_point(snt::date(2025, 4, 10),
                                              snt::hours(19) + snt::minutes(0));
    std::uint32_t ggStatus = ggService.get_cell_status(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}' << " -> status = " << ggStatus
	      << " [" << snrc::tracker_cell_status::status_to_string(ggStatus) << ']' << '\n';
  }

  {
    geomtools::geom_id ggGid(1203, 0, 1, 5, 66); // #1616 @ phase 2
    snt::time_point mapTime = snt::time_point(snt::date(2025, 7, 8),
                                              snt::hours(19) + snt::minutes(0));
    std::uint32_t ggStatus = ggService.get_cell_status(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}' << " -> status = " << ggStatus
	      << " [" << snrc::tracker_cell_status::status_to_string(ggStatus) << ']' << '\n';
  }

  ggService.reset();
  return;
}
