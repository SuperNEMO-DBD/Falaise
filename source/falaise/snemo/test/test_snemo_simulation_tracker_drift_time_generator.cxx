// test_snemo_simulation_tracker_drift_time_generator.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/mygsl/rng.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/simulation/tracker_drift_time_generator.hpp>

void test1();

int main(int /*argc_*/, char** /*argv_*/)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::simulation::tracker_drift_time_generator'!" << std::endl;
    test1();

    std::clog << "The end." << std::endl;
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
  namespace snt = snemo::time;
  namespace snsim = snemo::simulation;
  
  // Service manager:
  datatools::properties serviceManagerConfig;
  serviceManagerConfig.store("logging", "debug");
  serviceManagerConfig.store_flag("force_initialization_at_load");
  std::vector<std::string> servicesConfigFiles = {
    "@falaise:snemo/demonstrator/geometry/5.0/GeometryServices.conf",   
    "@falaise:snemo/demonstrator/reconstruction/3.0/services/TrackerCellStatusService.conf"};
  serviceManagerConfig.store("services.configuration_files", servicesConfigFiles);
  datatools::service_manager serviceManager;
  serviceManager.initialize(serviceManagerConfig);

  datatools::properties tdrGenConfig;
  tdrGenConfig.store("drift_model_name", "betsy");
  tdrGenConfig.store("drift_model.model", "betsy");
  tdrGenConfig.store("drift_model.version", 0);
  tdrGenConfig.store_with_explicit_unit("mean_ionization_energy", 50.0 * CLHEP::eV);
  tdrGenConfig.set_unit_symbol("mean_ionization_energy", "eV");
 
  snsim::tracker_drift_time_generator tdrGen(tdrGenConfig, serviceManager.get_local_services());

  {
    geomtools::geom_id cellGid(1204, 0, 0, 0, 0);
    snt::time_point evenTimestamp = snt::time_point_from_string("2024-04-11 09:45:00");
    geomtools::vector_3d startPos(22. * CLHEP::mm, 12.0 * CLHEP::mm, 0.0);
    geomtools::vector_3d stopPos(-22. * CLHEP::mm, 5.0 * CLHEP::mm, 0.0);
    double startTime = 0.0 * CLHEP::ns;
    double stopTime = 0.2 * CLHEP::ns;
    double energyDeposit = 2.4 * CLHEP::keV;
    std::string rng_id = mygsl::rng::DEFAULT_RNG_ID;
    int32_t seed = 12345;
    mygsl::rng random;
    random.initialize(rng_id, seed);

    std::ofstream fout("test-tracker_drift_time_generator-random.data");
    auto nSteps = 100000u;
    for (auto i = 0u; i < nSteps; i++) {
      double driftTime = tdrGen.generate_drift_time(cellGid, evenTimestamp,
						    startPos, stopPos,
						    startTime, stopTime,
						    energyDeposit,
						    random);
      fout << driftTime / CLHEP::ns << '\n';	
    }
    fout << '\n';
    fout.close();    
  }
  
  serviceManager.reset();
  return;
}
