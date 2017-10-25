// test_base_topology_pattern.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <falaise/snemo/datamodels/topology_2e_pattern.h>
#include <falaise/snemo/datamodels/tof_measurement.h>

int main()
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'base_topology_pattern' class." << std::endl;

    // Create a fake topology pattern :
    snemo::datamodel::base_topology_pattern::handle_type hTP0;
    hTP0.reset(new snemo::datamodel::topology_2e_pattern);
    snemo::datamodel::base_topology_pattern & a_pattern = hTP0.grab();

    // Add associated measurement :
    snemo::datamodel::base_topology_pattern::measurement_dict_type & meas_dict
      = a_pattern.grab_measurement_dictionary();
    // Add fake TOF measurements
    meas_dict.insert(std::make_pair("fake_tof_1", new snemo::datamodel::tof_measurement));
    meas_dict.insert(std::make_pair("fake_tof_2", new snemo::datamodel::tof_measurement));
    meas_dict.insert(std::make_pair("fake_tof_3", new snemo::datamodel::tof_measurement));
    meas_dict.insert(std::make_pair("fake_tof_10", new snemo::datamodel::tof_measurement));

    a_pattern.tree_dump();

    // Check measurement existence (through regular expression)
    std::vector<std::string> keys = {
      "fake_tof",
      "fake_tof_.?",
      "fake_tof_[0-9]+",
      "fake_tof_[0-9]{2}",
      "fake_tof_[0-9]{3}",
      ".*",
      ".*_(1|10)",
      ".*_(100|1000)"
    };
    for (size_t i = 0; i < keys.size(); i++) {
      const std::string & a_key = keys.at(i);
      std::clog << "Topology pattern has ";
      if (! a_pattern.has_measurement(a_key)) {
        std::clog << "no ";
      }
      std::clog << "'" << a_key << "' measurement" << std::endl;
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
