// Standard library:
#include <iostream>
#include <cstdlib>

// Bayeux:
#include <datatools/clhep_units.h>
#include <genbb_help/primary_event.h>
#include <mygsl/rng.h>
#include <mygsl/histogram.h>

// This project:
#include <exaegir/dummy_event_generator.h>
#include <exaegir/dummy_event_generator2.h>

void run1();
void run2();

int main(void)
{
  int error_code = EXIT_SUCCESS;
  // run1();
  run2();
  exit(EXIT_SUCCESS);
}

void run1()
{
  int error_code = EXIT_SUCCESS;

  // Random number generator:
  int32_t prngSeed(314159); 
  mygsl::rng prng(prngSeed);
  
  // Event generator configuration:
  datatools::properties dummyEvGenConfig;
  
  // Read the event generator configuration file:
  std::string dummyEvGenConfigPath = "@exaegir:config/config1.conf";
  datatools::fetch_path_with_env(dummyEvGenConfigPath);
  datatools::properties::read_config(dummyEvGenConfigPath, dummyEvGenConfig);
  {
    std::cerr << "[debug] Event generator configuration:" << std::endl;
    boost::property_tree::ptree pOptions;
    pOptions.put("indent", "[debug] ");
    dummyEvGenConfig.print_tree(std::cerr, pOptions);
  }
  
  // The event generator:
  exaegir::dummy_event_generator dummyEvGen;
  dummyEvGen.set_external_random(prng);

  // Initialize the event generator:
  dummyEvGen.initialize_standalone(dummyEvGenConfig);

  // Use the event generator:
  genbb::primary_event event;
  bool computeClassification = true;
  unsigned int nbEvents = 5;
  for (unsigned int iEvent = 0; iEvent < nbEvents; iEvent++) {
    std::clog << "Event #" << iEvent << std::endl;
    dummyEvGen.load_next(event, computeClassification);
    event.print_tree(std::cerr);
  }

  // Terminate the event generator:
  dummyEvGen.reset();
  
  exit(EXIT_SUCCESS);
}

void run2()
{
  int error_code = EXIT_SUCCESS;

  // Random number generator:
  int32_t prngSeed(314159); 
  mygsl::rng prng(prngSeed);
  
  // Event generator configuration:
  datatools::properties dummyEvGenConfig;
  
  // Read the event generator configuration file:
  std::string dummyEvGenConfigPath = "@exaegir:config/config3.conf";
  datatools::fetch_path_with_env(dummyEvGenConfigPath);
  datatools::properties::read_config(dummyEvGenConfigPath, dummyEvGenConfig);
  {
    std::cerr << "[debug] Event generator configuration:" << std::endl;
    boost::property_tree::ptree pOptions;
    pOptions.put("indent", "[debug] ");
    dummyEvGenConfig.print_tree(std::cerr, pOptions);
  }
  
  // The event generator:
  exaegir::dummy_event_generator2 dummyEvGen;
  dummyEvGen.set_external_random(prng);

  // Initialize the event generator:
  dummyEvGen.initialize_standalone(dummyEvGenConfig);

  // Set a 1D-histogram for the electron kinetic energy:
  mygsl::histogram h0(100, 0.0, 3.0);
  
  // Use the event generator:
  genbb::primary_event event;
  bool computeClassification = true;
  unsigned int nbEvents = 10000000;
  for (unsigned int iEvent = 0; iEvent < nbEvents; iEvent++) {
    if (iEvent % 100000 == 0) std::clog << "Event #" << iEvent << std::endl;
    dummyEvGen.load_next(event, computeClassification);
    // Stat the kinetic energy of the first particle in the event:
    h0.fill(event.get_particles().front().get_kinetic_energy() / CLHEP::MeV);
  }

  // ASCII display of the histogram:
  h0.print_ascii(std::cerr);
 
  {
    std::string sname = "runexaegir2.his";
    std::ofstream ofstore(sname.c_str());
    h0.print(ofstore);
    ofstore.close();
  }

  // Terminate the event generator:
  dummyEvGen.reset();
  
  exit(EXIT_SUCCESS);
}
