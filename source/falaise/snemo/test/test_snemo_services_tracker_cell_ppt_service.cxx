// test_snemo_services_tracker_cell_ppt_service.cxx

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
#include <falaise/snemo/services/tracker_cell_ppt_service.h>
#include <falaise/snemo/datamodels/geomid_utils.h>

void test_init_services(datatools::service_manager & service_manager_,
			const datatools::logger::priority verbosity_ = datatools::logger::PRIO_FATAL);
void test1();
void test2();
void test3();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::service::tracker_cell_ppt_service'!" << std::endl;
    test1();
    test2();
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
  datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
  // verbosity = datatools::logger::PRIO_DEBUG;
  
  datatools::service_manager serviceMgr;
  test_init_services(serviceMgr, verbosity);
  
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;
  datatools::properties ggPptServiceConfig;
  ggPptServiceConfig.store("mode", "fallback");
  ggPptServiceConfig.store("tracker_cell_status_label", snemo::service_info::trackerCellStatusServiceName());

  snemo::tracker_cell_ppt_service ggPptService;
  ggPptService.set_logging_priority(verbosity);
  ggPptService.initialize(ggPptServiceConfig,
			  const_cast<datatools::service_dict_type&>(serviceMgr.get_local_services()));
  if (datatools::logger::is_debug(verbosity)) {
    boost::property_tree::ptree printOpts;
    printOpts.put("indent", "[debug] ");
    ggPptService.print_tree(std::clog, printOpts);
  }
    
  {
    geomtools::geom_id ggGid(1203, 0, 0, 0, 0);
    snt::time_point mapTime = snt::time_point(snt::date(2025, 4, 10),
                                              snt::hours(19) + snt::minutes(0));
    snemo::rc::tracker_cell_ppt_info ggPptInfo = ggPptService.get_cell_ppt_info(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}'
	      << " -> PPR info = " << ggPptInfo  << '\n';
  }
 
  ggPptService.reset();
  return;
}

void test2()
{
  std::clog << "\nTest 2:\n";
  datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
  // verbosity = datatools::logger::PRIO_DEBUG;
  
  datatools::service_manager serviceMgr;
  test_init_services(serviceMgr, verbosity);
  
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;
  datatools::properties ggPptServiceConfig;
  ggPptServiceConfig.store("mode", "db");
  ggPptServiceConfig.store("tracker_cell_status_label", snemo::service_info::trackerCellStatusServiceName());
  ggPptServiceConfig.store("db_label", snemo::service_info::dbServiceName());
  ggPptServiceConfig.store("db.table_name", "GG_PPT");

  snemo::tracker_cell_ppt_service ggPptService;
  ggPptService.set_logging_priority(verbosity);
  ggPptService.initialize(ggPptServiceConfig,
			  const_cast<datatools::service_dict_type&>(serviceMgr.get_local_services()));
  if (datatools::logger::is_debug(verbosity)) {
    boost::property_tree::ptree printOpts;
    printOpts.put("indent", "[debug] ");
    ggPptService.print_tree(std::clog, printOpts);
  }
    
  {
    geomtools::geom_id ggGid(1203, 0, 0, 0, 0);
    snt::time_point mapTime = snt::time_point(snt::date(2025, 4, 10),
                                              snt::hours(19) + snt::minutes(0));
    snemo::rc::tracker_cell_ppt_info ggPptInfo = ggPptService.get_cell_ppt_info(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}'
	      << " -> PPR info = " << ggPptInfo  << '\n';
  }
 
  ggPptService.reset();
  return;
}

void test3()
{
  std::clog << "\nTest 3:\n";
  datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
  // verbosity = datatools::logger::PRIO_DEBUG;
  
  datatools::service_manager serviceMgr;
  test_init_services(serviceMgr, verbosity);
  
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;
  datatools::properties ggPptServiceConfig;
  ggPptServiceConfig.store("mode", "files");
  ggPptServiceConfig.store("tracker_cell_status_label", snemo::service_info::trackerCellStatusServiceName());
  std::vector<std::string> mapPaths = {
    "@falaise:snemo/demonstrator/data/calibration/tracker/tracker_cell_ppt_map_mock-1.csv",
    "@falaise:snemo/demonstrator/data/calibration/tracker/tracker_cell_ppt_map_mock-2.csv"    
  };
  ggPptServiceConfig.store("files.cell_maps", mapPaths);
  ggPptServiceConfig.store("files.map_format", "csv-1"); // default value for the supported format

  snemo::tracker_cell_ppt_service ggPptService;
  ggPptService.set_logging_priority(verbosity);
  ggPptService.initialize(ggPptServiceConfig,
			  const_cast<datatools::service_dict_type&>(serviceMgr.get_local_services()));
  if (datatools::logger::is_debug(verbosity)) {
    boost::property_tree::ptree printOpts;
    printOpts.put("indent", "[debug] ");
    ggPptService.print_tree(std::clog, printOpts);
  }
    
  {
    geomtools::geom_id ggGid(1203, 0, 0, 0, 0);
    snt::time_point mapTime = snt::time_point(snt::date(2025, 4, 10),
                                              snt::hours(19) + snt::minutes(0));
    snemo::rc::tracker_cell_ppt_info ggPptInfo = ggPptService.get_cell_ppt_info(ggGid, mapTime);
    std::cout << "@" << mapTime << " GG #" << ggGid << " {#" << snemo::datamodel::gg_num(ggGid) << '}'
	      << " -> PPR info = " << ggPptInfo  << '\n';
  }
 
  ggPptService.reset();
  return;
}

void test_init_services(datatools::service_manager & service_manager_, datatools::logger::priority verbosity_)
{
  std::string dbServiceMgrConfigPath = "@falaise:snemo/demonstrator/db/0.1/services/DbService.conf";
  datatools::fetch_path_with_env(dbServiceMgrConfigPath);
  datatools::multi_properties dbServiceMgrConfig("name", "type");
  dbServiceMgrConfig.read(dbServiceMgrConfigPath);
 
  std::string geomServiceMgrConfigPath = "@falaise:snemo/demonstrator/geometry/6.0/GeometryServices.conf";
  datatools::fetch_path_with_env(geomServiceMgrConfigPath);
  datatools::multi_properties geomServiceMgrConfig("name", "type");
  geomServiceMgrConfig.read(geomServiceMgrConfigPath);

  std::string ggStatusServiceMgrConfigPath = "@falaise:snemo/demonstrator/reconstruction/4.0/services/TrackerCellStatusService.conf";
  datatools::fetch_path_with_env(ggStatusServiceMgrConfigPath);
  datatools::multi_properties ggStatusServiceMgrConfig("name", "type");
  ggStatusServiceMgrConfig.read(ggStatusServiceMgrConfigPath);
  
  auto & serviceMgr = service_manager_;
  if (serviceMgr.is_initialized()) {
    serviceMgr.reset();
  }
  serviceMgr.set_logging_priority(verbosity_);
  serviceMgr.load(dbServiceMgrConfig);
  serviceMgr.load(geomServiceMgrConfig);
  serviceMgr.load(ggStatusServiceMgrConfig);
  serviceMgr.initialize();

  return;
}
