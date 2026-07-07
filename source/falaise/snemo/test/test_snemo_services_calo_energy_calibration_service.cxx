// test_snemo_services_calo_energy_calibration_service.cxx

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
#include <falaise/snemo/services/calo_energy_calibration_service.h>

void test1();
void test2();

int main(int argc_, char** argv_)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::run_info_service'!" << std::endl;
    bool runTest1 = true;
    bool runTest2 = true;

    int iarg = 0;
    while (iarg < argc_) {
      std::string arg(argv_[iarg]);
      std::clog << "arg=" << arg << '\n';
      if (not arg.empty() and arg[0] == '-') {
	if (arg == "-1") runTest1 = true;
	else if (arg == "-2") runTest2 = true;
	// else if (arg == "-3") runTest3 = true;
	else throw std::logic_error("invalid option");
      }
      iarg++;
    }
     
    if (runTest1) test1();
    if (runTest2) test2();
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
   
  std::string dbServiceMgrConfigPath = "@falaise:snemo/demonstrator/db/0.1/services/DbService.conf";
  datatools::fetch_path_with_env(dbServiceMgrConfigPath);
  datatools::multi_properties dbServiceMgrConfig("name", "type");
  dbServiceMgrConfig.read(dbServiceMgrConfigPath);
  
  datatools::service_manager serviceMgr;
  // serviceMgr.set_logging_priority(datatools::logger::PRIO_DEBUG);
  serviceMgr.load(dbServiceMgrConfig);
  serviceMgr.initialize();

  {
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
    // runInfoServiceConfig.tree_dump(std::clog, "Run info service :");
    // runInfoService.set_logging_priority(datatools::logger::PRIO_DEBUG);
    runInfoService.initialize(runInfoServiceConfig, snemo::service_bus(serviceMgr));
    // runInfoService.tree_dump(std::clog, "Run info service :");

    {
      snemo::calo_energy_calibration_service caloEnergyCalibService;
      datatools::properties caloEnergyCalibServiceConfig;
      //caloEnergyCalibServiceConfig.store("use_fallback_calibrator", true);
      caloEnergyCalibServiceConfig.store("calibrator_type", "pol1");
      caloEnergyCalibServiceConfig.store("data_access_mode", "files");
      
      caloEnergyCalibService.set_logging_priority(datatools::logger::PRIO_DEBUG);
      caloEnergyCalibService.set_run_info_service(runInfoService);
      caloEnergyCalibService.initialize(caloEnergyCalibServiceConfig, snemo::service_bus(serviceMgr));
      {
	boost::property_tree::ptree printOpts;
	printOpts.put("title","Calorimeter energy calibration service");
	caloEnergyCalibService.print_tree(std::clog, printOpts);
      }

      namespace snt = snemo::time;
      {
 	// Run #2027 cat=3 start=2025-05-26 17:06:14.771048 duration=43201 s
	snt::time_point hitTimepoint(snt::date(2025, 5, 27),
				     snt::hours(2) +  snt::minutes(13)
				     + snt::seconds(42) + snt::milliseconds(678));
	int omNum = 125;
	auto calibratorHdl = caloEnergyCalibService.get_calibrator_handle(hitTimepoint, omNum);
	if (calibratorHdl) {
	  snemo::datamodel::precalibrated_calorimeter_hit caloHit;
	  caloHit.set_hit_id(0);
	  caloHit.set_amplitude(0.2 * CLHEP::volt);
	  caloHit.set_sigma_amplitude(0.01 * CLHEP::volt);
	  caloHit.set_charge(4.0 * CLHEP::volt * CLHEP::nanosecond);
	  caloHit.set_sigma_charge(0.05 * CLHEP::volt * CLHEP::nanosecond);	  
	  snemo::processing::calo_energy caloEnergy = calibratorHdl->calibrate(caloHit);
	  std::clog << "Energy = " << caloEnergy.value << " +/- " << caloEnergy.sigma << std::endl;
	} else {
	  std::clog << "Missing energy calibration for OM #" << omNum << std::endl;
	}
      }
      
      {
	std::clog << "\n\n";
	// Run #2683 cat=3 start=2025-07-09 19:55:10.546672 duration=43201 s
	snt::time_point hitTimepoint(snt::date(2025, 7, 9),
				     snt::hours(20) +  snt::minutes(13)
				     + snt::seconds(42) + snt::milliseconds(678));
	int omNum = 125;
	auto calibratorHdl = caloEnergyCalibService.get_calibrator_handle(hitTimepoint, omNum);
	
	if (calibratorHdl) {
	  snemo::datamodel::precalibrated_calorimeter_hit caloHit;
	  caloHit.set_hit_id(0);
	  caloHit.set_amplitude(0.2 * CLHEP::volt);
	  caloHit.set_sigma_amplitude(0.01 * CLHEP::volt);
	  caloHit.set_charge(4.0 * CLHEP::volt * CLHEP::nanosecond);
	  caloHit.set_sigma_charge(0.05 * CLHEP::volt * CLHEP::nanosecond);	
	  snemo::processing::calo_energy caloEnergy = calibratorHdl->calibrate(caloHit);
	  std::clog << "Energy = " << caloEnergy.value << " +/- " << caloEnergy.sigma << std::endl;
	} else {
	  std::clog << "Missing energy calibration for OM #" << omNum << std::endl;
	}
      }
  
      {
	std::clog << "\n\n";
	namespace snt = snemo::time;
	// Run #2686 cat=3 start=2025-07-10 10:25:16.948561 duration=21602.1 s
	snt::time_point hitTimepoint(snt::date(2025, 7, 10),
				     snt::hours(11) +  snt::minutes(13)
				     + snt::seconds(42) + snt::milliseconds(678));
	int omNum = 125;
	auto calibratorHdl = caloEnergyCalibService.get_calibrator_handle(hitTimepoint, omNum);
	
	if (calibratorHdl) {
	  snemo::datamodel::precalibrated_calorimeter_hit caloHit;
	  caloHit.set_hit_id(0);
	  caloHit.set_amplitude(0.2 * CLHEP::volt);
	  caloHit.set_sigma_amplitude(0.01 * CLHEP::volt);
	  caloHit.set_charge(4.0 * CLHEP::volt * CLHEP::nanosecond);
	  caloHit.set_sigma_charge(0.05 * CLHEP::volt * CLHEP::nanosecond);
	  snemo::processing::calo_energy caloEnergy = calibratorHdl->calibrate(caloHit);
	  std::clog << "Energy = " << caloEnergy.value << " +/- " << caloEnergy.sigma << std::endl;
	} else {
	  std::clog << "Missing energy calibration for OM #" << omNum << std::endl;
	}
      }
     
      caloEnergyCalibService.reset();
    }
    
    runInfoService.reset();
  }
  
  serviceMgr.reset();
  return;
}

void test2()
{
  std::clog << "\nTest 2:\n";
  namespace snt = snemo::time;
   
  std::string dbServiceMgrConfigPath = "@falaise:snemo/demonstrator/db/0.1/services/DbService.conf";
  datatools::fetch_path_with_env(dbServiceMgrConfigPath);
  datatools::multi_properties dbServiceMgrConfig("name", "type");
  dbServiceMgrConfig.read(dbServiceMgrConfigPath);
  
  datatools::service_manager serviceMgr;
  // serviceMgr.set_logging_priority(datatools::logger::PRIO_DEBUG);
  serviceMgr.load(dbServiceMgrConfig);
  serviceMgr.initialize();

  {
    snemo::calo_energy_calibration_service caloEnergyCalibService;
    datatools::properties caloEnergyCalibServiceConfig;
    caloEnergyCalibServiceConfig.store("data_access_mode", "db");
    caloEnergyCalibServiceConfig.store("db.table_name", "OM_Energy_Calib");
    caloEnergyCalibServiceConfig.store_flag("db.debug");
    // caloEnergyCalibServiceConfig.store_flag("use_fallback_calibrator");
       
    caloEnergyCalibService.set_logging_priority(datatools::logger::PRIO_DEBUG);
    caloEnergyCalibService.initialize(caloEnergyCalibServiceConfig, snemo::service_bus(serviceMgr));
    {
      boost::property_tree::ptree printOpts;
      printOpts.put("title", "Calorimeter energy calibration service");
      caloEnergyCalibService.print_tree(std::clog, printOpts);
    }

    {
      snt::time_point hitTimepoint(snt::date(2020, 1, 1),
				   snt::hours(2) +  snt::minutes(13)
				   + snt::seconds(42) + snt::milliseconds(678));
      int omNum = 0;
      auto calibratorHdl = caloEnergyCalibService.get_calibrator_handle(hitTimepoint, omNum);
	
      if (calibratorHdl) {
	snemo::datamodel::precalibrated_calorimeter_hit caloHit;
	caloHit.set_hit_id(0);
	caloHit.set_amplitude(0.2 * CLHEP::volt);
	caloHit.set_sigma_amplitude(0.01 * CLHEP::volt);
	caloHit.set_charge(4.0 * CLHEP::volt * CLHEP::nanosecond);
	caloHit.set_sigma_charge(0.05 * CLHEP::volt * CLHEP::nanosecond);    	
	snemo::processing::calo_energy caloEnergy = calibratorHdl->calibrate(caloHit);
	std::clog << "Energy = " << caloEnergy.value << " +/- " << caloEnergy.sigma << std::endl;
      } else {
	std::cerr << "[error] Missing energy calibration for OM #" << omNum << std::endl;
      }
    }

    {
      snt::time_point hitTimepoint(snt::date(2020, 1, 1),
				   snt::hours(2) +  snt::minutes(13)
				   + snt::seconds(42) + snt::milliseconds(678));
      int omNum = 0;
      auto calibratorHdl = caloEnergyCalibService.get_calibrator_handle(hitTimepoint, omNum);
	
      if (calibratorHdl) {
	snemo::datamodel::precalibrated_calorimeter_hit caloHit;
	caloHit.set_hit_id(0);
	caloHit.set_amplitude(0.2 * CLHEP::volt);
	caloHit.set_sigma_amplitude(0.01 * CLHEP::volt);
	caloHit.set_charge(4.0 * CLHEP::volt * CLHEP::nanosecond);
	caloHit.set_sigma_charge(0.05 * CLHEP::volt * CLHEP::nanosecond);    	
	snemo::processing::calo_energy caloEnergy = calibratorHdl->calibrate(caloHit);
	std::clog << "Energy = " << caloEnergy.value / CLHEP::MeV << " +/- " << caloEnergy.sigma / CLHEP::MeV << std::endl;
      } else {
	std::cerr << "[error] Missing energy calibration for OM #" << omNum << std::endl;
      }
    }

    {
      snt::time_point hitTimepoint(snt::date(2023, 6, 10),
				   snt::hours(2) +  snt::minutes(13)
				   + snt::seconds(42) + snt::milliseconds(678));
      int omNum = 1;
      auto calibratorHdl = caloEnergyCalibService.get_calibrator_handle(hitTimepoint, omNum);
	
      if (calibratorHdl) {
	snemo::datamodel::precalibrated_calorimeter_hit caloHit;
	caloHit.set_hit_id(0);
	caloHit.set_amplitude(0.2 * CLHEP::volt);
	caloHit.set_sigma_amplitude(0.01 * CLHEP::volt);
	caloHit.set_charge(4.0 * CLHEP::volt * CLHEP::nanosecond);
	caloHit.set_sigma_charge(0.05 * CLHEP::volt * CLHEP::nanosecond);    	
	snemo::processing::calo_energy caloEnergy = calibratorHdl->calibrate(caloHit);
	std::clog << "Energy = " << caloEnergy.value / CLHEP::MeV << " +/- " << caloEnergy.sigma / CLHEP::MeV << std::endl;
      } else {
	std::cerr << "[error] Missing energy calibration for OM #" << omNum << std::endl;
      }
    }

    {
      snt::time_point hitTimepoint(snt::date(2023, 6, 10),
				   snt::hours(2) +  snt::minutes(13)
				   + snt::seconds(42) + snt::milliseconds(678));
      int omNum = 3 ;
      auto calibratorHdl = caloEnergyCalibService.get_calibrator_handle(hitTimepoint, omNum);
      if (not calibratorHdl) {
	std::cerr << "[info] As expected : Missing energy calibration for OM #" << omNum << " for time " << snt::to_string(hitTimepoint) << std::endl;
      }
    }

    caloEnergyCalibService.reset();
  }

  serviceMgr.reset();
  return;
}
