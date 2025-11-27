// test_snemo_services_bus.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Bayeux:
#include <bayeux/datatools/service_manager.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/services/service_handle.h>
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/services/run_info_service.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  falaise::initialize();
  try {
    std::clog << "Test program for class 'snemo::run_info_service'!" << std::endl;
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

  std::string dbServiceMgrConfigPath
    = "@falaise:snemo/demonstrator/db/0.1/services/DbService.conf";
  datatools::fetch_path_with_env(dbServiceMgrConfigPath);
  datatools::multi_properties dbServiceMgrConfig("name", "type");
  dbServiceMgrConfig.read(dbServiceMgrConfigPath);
 
  std::string runInfoServiceMgrConfigPath
    = "@falaise:snemo/demonstrator/reconstruction/3.1/services/RunInfoService.conf";
  datatools::fetch_path_with_env(runInfoServiceMgrConfigPath);
  datatools::multi_properties runInfoServiceMgrConfig("name", "type");
  runInfoServiceMgrConfig.read(runInfoServiceMgrConfigPath);
  
  datatools::service_manager serviceMgr;
  serviceMgr.set_logging_priority(datatools::logger::PRIO_DEBUG);
  serviceMgr.load(dbServiceMgrConfig);
  serviceMgr.load(runInfoServiceMgrConfig);
  serviceMgr.initialize();
  
  {
    serviceMgr.tree_dump(std::clog, "SuperNEMO service manager", ">>> ");

    // Access the DB service:
    snemo::service_handle<snemo::db_service> dbServiceHandle{serviceMgr};
    {
      boost::property_tree::ptree popts;
      popts.put("title", "Db service");
      popts.put("indent", "1 > ");
      dbServiceHandle.instance()->print_tree(std::clog, popts);
    }

    // Access the run info service:
    snemo::service_handle<snemo::run_info_service> runInfoServiceHandle{serviceMgr};
    {
      boost::property_tree::ptree popts;
      popts.put("title", "Run info service");
      popts.put("indent", "2 > ");
      runInfoServiceHandle.instance()->print_tree(std::clog, popts);
    }

    
  }
  
  serviceMgr.reset();
  return;
}
