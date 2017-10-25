// test_topology_data.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <falaise/snemo/datamodels/topology_data.h>
#include <falaise/snemo/datamodels/topology_1e_pattern.h>
#include <falaise/snemo/datamodels/topology_2e_pattern.h>
#include <falaise/snemo/datamodels/tof_measurement.h>

int main()
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'topology_data' class." << std::endl;

    // Create a topology pattern :
    snemo::datamodel::topology_data::handle_pattern hP0;
    hP0.reset(new snemo::datamodel::topology_2e_pattern);
    // Add associated particle tracks :
    snemo::datamodel::base_topology_pattern::particle_track_dict_type & pt_dict
      = hP0.grab().grab_particle_track_dictionary();
    // Create 2 fake electrons :
    pt_dict.insert(std::make_pair("fake_electron0", new snemo::datamodel::particle_track));
    pt_dict.insert(std::make_pair("fake_electron1", new snemo::datamodel::particle_track));

    // Add associated measurement :
    snemo::datamodel::base_topology_pattern::measurement_dict_type & meas_dict
      = hP0.grab().grab_measurement_dictionary();
    // Add a fake TOF measurement
    meas_dict.insert(std::make_pair("fake_tof", new snemo::datamodel::tof_measurement));

    // Topology data bank :
    snemo::datamodel::topology_data TD;
    TD.set_pattern_handle(hP0);
    TD.grab_auxiliaries().store_flag("test_td");
    TD.tree_dump(std::clog, "Topology data:");

    // Get pattern handle
    if (TD.has_pattern()) {
      const snemo::datamodel::base_topology_pattern & a_btp
        = TD.get_pattern();
      std::cout << "Pattern id " << a_btp.get_pattern_id() << std::endl;
    }

    // Check existence of 2e pattern
    if (TD.has_pattern_as<snemo::datamodel::topology_2e_pattern>()) {
      std::cout << "TD has " << snemo::datamodel::topology_2e_pattern::pattern_id()
                << " pattern" << std::endl;
      const snemo::datamodel::topology_2e_pattern & t2ep
        = TD.get_pattern_as<snemo::datamodel::topology_2e_pattern>();
      t2ep.tree_dump(std::clog, "2e topology pattern:");
    }

    // Check non existence of 1e pattern
    try {
      const snemo::datamodel::topology_1e_pattern & t1ep
        = TD.get_pattern_as<snemo::datamodel::topology_1e_pattern>();
      t1ep.tree_dump();
    } catch (...) {
      std::cerr << "No 1e pattern stored!" << std::endl;
    }
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
