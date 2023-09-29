// test_precalibrated_calorimeter_hit.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/smart_ref.h>
#include <datatools/units.h>

// This project:
#include <falaise/snemo/datamodels/precalibrated_calorimeter_hit.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::precalibrated_calorimeter_hit'!"
              << std::endl;

    // bool debug = false;

    // int iarg = 1;
    // while (iarg < argc_) {
    //   std::string token = argv_[iarg];
    //   if (token[0] == '-') {
    //     std::string option = token;
    //     if ((option == "-d") || (option == "--debug")) {
    //       debug = true;
    //     } else {
    //       std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
    //     }
    //   } else {
    //     std::string argument = token;
    //     {
    //       std::clog << "warning: ignoring argument '"
    //                 << argument << "'!" << std::endl;
    //     }
    //   }
    //   iarg++;
    // }

    namespace sdm = snemo::datamodel;

    {
      sdm::precalibrated_calorimeter_hit my_calo_hit;
      my_calo_hit.set_hit_id(1);
      geomtools::geom_id gid(1301, 0, 1, 8, 6);
      my_calo_hit.set_geom_id(gid);
      my_calo_hit.set_baseline(0.913734 * CLHEP::volt);
      my_calo_hit.set_amplitude(-113.525 * 1E-3 * CLHEP::volt);
      my_calo_hit.set_charge(-3.1724 * 1E-9 * CLHEP::volt * CLHEP::second);
      my_calo_hit.set_time(22.836654123456 * CLHEP::second);
      my_calo_hit.grab_auxiliaries().store_real("time_cfd", 119.5 * CLHEP::nanosecond);
      my_calo_hit.tree_dump(std::clog, "\nSimple precalibrated calorimeter hit");
    }

    {
      sdm::precalibrated_calorimeter_hit my_calo_hit;
      my_calo_hit.set_hit_id(1);
      geomtools::geom_id gid(1301, 0, 1, 8, 6);
      my_calo_hit.set_geom_id(gid);
      my_calo_hit.set_baseline(0.913734 * CLHEP::volt);
      my_calo_hit.set_sigma_baseline(0.00913734 * CLHEP::volt);
      my_calo_hit.set_amplitude(-113.525 * 1E-3 * CLHEP::volt);
      my_calo_hit.set_sigma_amplitude(1.13525 * 1E-3 * CLHEP::volt);
      my_calo_hit.set_charge(-3.1724 * 1E-9 * CLHEP::volt * CLHEP::second);
      my_calo_hit.set_sigma_charge(0.031724 * 1E-9 * CLHEP::volt * CLHEP::second);
      my_calo_hit.set_time(22.836654123456 * CLHEP::second);
      my_calo_hit.set_sigma_time(0.22836654123 * 1E-9 * CLHEP::second);
      my_calo_hit.grab_auxiliaries().store_real("time_cfd", 119.5 * CLHEP::nanosecond);
      my_calo_hit.tree_dump(std::clog, "\nSimple precalibrated calorimeter hit with errors");
    }

    {
      // Create a vector of random calorimeter hits:
      srand48(314159);
      using hit_collection_type = std::vector<sdm::precalibrated_calorimeter_hit>;
      hit_collection_type list_of_calo_hits;

      size_t nb_hits = 3;
      list_of_calo_hits.reserve(nb_hits);
      for (int i = 0; i < (int)nb_hits; ++i) {
        // Append a new empty hit in the list:
        list_of_calo_hits.push_back(sdm::precalibrated_calorimeter_hit());
        // Get a reference to the newly pushed hit:
        sdm::precalibrated_calorimeter_hit& calo_hit = list_of_calo_hits.back();
        calo_hit.set_hit_id(156572 + i);
        geomtools::geom_id gid(1302,  // Type for the 'calorimeter_block' category
                               0,     // Module
                               (uint32_t)(drand48() * 2),    // Side
                               (uint32_t)(drand48() * 20),   // Column
                               (uint32_t)(drand48() * 13));  // Row
        calo_hit.set_geom_id(gid);
	calo_hit.set_baseline(drand48() * 0.913734 * CLHEP::volt);
	calo_hit.set_amplitude(drand48() * -113.525 * 1E-3 * CLHEP::volt);
	calo_hit.set_charge(drand48() * -3.1724 * 1E-9 * CLHEP::volt * CLHEP::second);
	calo_hit.set_time(drand48() * 22.836654123 * CLHEP::second);
	calo_hit.grab_auxiliaries().store_real("time_cfd", 119.5 * CLHEP::nanosecond);
        if (drand48() < 0.1) {
          calo_hit.grab_auxiliaries().store_flag("special_flag");
        }
      }

      // Search for hits with min/max amplitude :
      using smart_ref_type = datatools::smart_ref<sdm::precalibrated_calorimeter_hit>;
      smart_ref_type emin_ch_ref;
      smart_ref_type emax_ch_ref;
      for (hit_collection_type::const_iterator i = list_of_calo_hits.begin();
           i != list_of_calo_hits.end(); i++) {
        const sdm::precalibrated_calorimeter_hit& calo_hit = *i;
        std::ostringstream title;
        title << "\nCalorimeter hit #" << calo_hit.get_hit_id();
        calo_hit.tree_dump(std::clog, title.str());
        if (!emin_ch_ref.is_valid()) {
          emin_ch_ref.set(calo_hit);
        } else if (calo_hit.get_amplitude() > emin_ch_ref.get().get_amplitude()) {
          emin_ch_ref.set(calo_hit);
        }
        if (!emax_ch_ref.is_valid()) {
          emax_ch_ref.set(calo_hit);
        } else if (calo_hit.get_amplitude() < emax_ch_ref.get().get_amplitude()) {
          emax_ch_ref.set(calo_hit);
        }
      }

      emin_ch_ref.get_properties().store_flag("min_energy");
      emax_ch_ref.get_properties().store_flag("max_energy");
      emin_ch_ref.get().tree_dump(std::clog, "\nPrecalibrated calorimeter hit with min amplitude:");
      emax_ch_ref.get().tree_dump(std::clog, "\nPrecalibrated calorimeter hit with max amplitude:");
    }

    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
