// test_snemo_services_run_info_service.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Bayeux:
#include <bayeux/datatools/service_manager.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/services/run_info_service.h>

void test1();
void test2();
void test3();

int main(int argc_, char** argv_)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::run_info_service'!" << std::endl;
    bool runTest1 = true;
    bool runTest2 = true;
    bool runTest3 = true;

    int iarg = 0;
    while (iarg < argc_) {
      std::string arg(argv_[iarg]);
      std::clog << "arg=" << arg << '\n';
      if (not arg.empty() and arg[0] == '-') {
	if (arg == "-1") runTest1 = true;
	else if (arg == "-2") runTest2 = true;
	else if (arg == "-3") runTest3 = true;
	else throw std::logic_error("invalid option");
      }
      iarg++;
    }
     
    if (runTest1) test1();
    if (runTest2) test2();
    if (runTest3) test3();
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
   
  namespace snrc = snemo::rc;
  snemo::run_info_service runInfoService;
  datatools::properties runInfoServiceConfig;
  runInfoServiceConfig.store("mode", "files");
  std::vector<std::string> runInfoMapFiles;
  runInfoMapFiles = { \
    "${FALAISE_SNEMO_TESTING_DIR}/config/test-run_list1.conf",
    "${FALAISE_SNEMO_TESTING_DIR}/config/test-run_list2.conf" \
  };
  runInfoServiceConfig.store("files.run_lists", runInfoMapFiles);
  runInfoService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  runInfoService.initialize_standalone(runInfoServiceConfig);
  runInfoService.print_tree(std::clog);
  runInfoService.reset();
  return;
}

void test2()
{
  std::clog << "\nTest 2:\n";
 
  namespace snrc = snemo::rc;
  snemo::run_info_service runInfoService;
  datatools::properties runInfoServiceConfig;
  runInfoServiceConfig.store("mode", "csv_files");
  std::vector<std::string> runInfoMapCsvFiles;
  runInfoMapCsvFiles = { \
    "${FALAISE_SNEMO_TESTING_DIR}/config/test-run_list_2025.csv"
  };
  runInfoServiceConfig.store("csv_files.run_lists", runInfoMapCsvFiles);
  runInfoService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  runInfoService.initialize_standalone(runInfoServiceConfig);
  runInfoService.print_tree(std::clog);
  runInfoService.reset();
  return;
}

void test3()
{
  std::clog << "\nTest 3:\n";

  std::string dbServiceMgrConfigPath = "@falaise:snemo/demonstrator/db/0.1/services/DbService.conf";
  datatools::fetch_path_with_env(dbServiceMgrConfigPath);
  datatools::multi_properties dbServiceMgrConfig("name", "type");
  dbServiceMgrConfig.read(dbServiceMgrConfigPath);
  
  datatools::service_manager serviceMgr;
  serviceMgr.set_logging_priority(datatools::logger::PRIO_DEBUG);
  serviceMgr.load(dbServiceMgrConfig);
  serviceMgr.initialize();
 
  namespace snrc = snemo::rc;
  snemo::run_info_service runInfoService;
  datatools::properties runInfoServiceConfig;
  runInfoServiceConfig.store("mode", "db");
  runInfoServiceConfig.store("db_label", "db");
  runInfoServiceConfig.store("db.run_infos.table_name", "Run_infos");
  runInfoServiceConfig.store("db.run_phases.table_name", "Run_phases");
  std::uint32_t runMin = 0u;
  std::uint32_t runMax = 1000000u;
  runInfoServiceConfig.store_integer("db.run_min", runMin);
  runInfoServiceConfig.store_integer("db.run_max", runMax);
  runInfoServiceConfig.tree_dump(std::clog, "Run info service :");
  
  runInfoService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  runInfoService.initialize(runInfoServiceConfig, snemo::service_bus(serviceMgr));
  
  runInfoService.tree_dump(std::clog, "Run info service :");

  if (runInfoService.has_run_info(1546)) {
    const auto & runInfo1546 = runInfoService.get_run_info(1546);
    {
      boost::property_tree::ptree props;
      props.put("indent", ">>> ");
      props.put("title", "RUN #"+std::to_string(1546));
      runInfo1546.print_tree(std::clog, props);
    }

    if (runInfoService.has_run_phase(0)) {
      const auto & runPhase0 = runInfoService.get_run_phase(0);
      {
	boost::property_tree::ptree props;
	props.put("indent", ">>> ");
	props.put("title", "RUN PHASE #"+std::to_string(0));
	runPhase0.print_tree(std::clog, props);
      }
      if (runPhase0.has_run(runInfo1546)) {
	std::clog << "Run#1546 belongs to run phase#0\n";
      }
    }    
	
  }

  runInfoService.reset();

  serviceMgr.reset();
  return;
}
