// test_calibrated_calorimeter_hit.cxx

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
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::calibrated_calorimeter_hit'!"
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
      sdm::calibrated_calorimeter_hit my_calo_hit;
      my_calo_hit.set_hit_id(2125715);
      geomtools::geom_id gid(1302, 0, 1, 1, 4);
      my_calo_hit.set_geom_id(gid);
      my_calo_hit.grab_auxiliaries().store_flag("noisy_pmt");
      my_calo_hit.set_time(1.23 * CLHEP::ns);
      my_calo_hit.set_sigma_time(257.0 * CLHEP::picosecond);
      my_calo_hit.set_energy(456. * CLHEP::keV);
      my_calo_hit.set_sigma_energy(37. * CLHEP::keV);
      my_calo_hit.tree_dump(std::clog, "Calibrated calorimeter hit");
    }  // namespace sdm=snemo::datamodel;

    {
      // Create a vector of random calorimeter hits:
      srand48(314159);
      using hit_collection_type = std::vector<sdm::calibrated_calorimeter_hit>;
      hit_collection_type list_of_calo_hits;
      size_t nb_hits = 3;
      list_of_calo_hits.reserve(nb_hits);
      for (int i = 0; i < (int)nb_hits; ++i) {
        // Append a new empty hit in the list:
        sdm::calibrated_calorimeter_hit a_hit;
        list_of_calo_hits.push_back(a_hit);
        // Get a reference to the newly pushed hit:
        sdm::calibrated_calorimeter_hit& calo_hit = list_of_calo_hits.back();
        calo_hit.set_hit_id(156572 + i);
        geomtools::geom_id gid(1302,  // Type for the 'calorimeter_block' category
                               0,     // Module
                               (uint32_t)(drand48() * 2),    // Side
                               (uint32_t)(drand48() * 14),   // Column
                               (uint32_t)(drand48() * 20));  // Row
        calo_hit.set_geom_id(gid);
        if (drand48() < 0.1) {
          calo_hit.grab_auxiliaries().store_flag("noisy_pmt");
        }
        calo_hit.set_time(drand48() * 4.0 * CLHEP::ns);
        calo_hit.set_sigma_time(300.0 * CLHEP::picosecond);
        calo_hit.set_energy(drand48() * 1500.0 * CLHEP::keV);
        calo_hit.set_sigma_energy(100.0 * (1. + drand48()) * CLHEP::keV);
      }

      // Search for hits with min/max energy :
      using smart_ref_type = datatools::smart_ref<sdm::calibrated_calorimeter_hit>;
      smart_ref_type emin_ch_ref;
      smart_ref_type emax_ch_ref;
      for (hit_collection_type::const_iterator i = list_of_calo_hits.begin();
           i != list_of_calo_hits.end(); i++) {
        const sdm::calibrated_calorimeter_hit& calo_hit = *i;
        std::ostringstream title;
        title << "Calorimeter hit #" << calo_hit.get_hit_id();
        calo_hit.tree_dump(std::clog, title.str());
        if (!emin_ch_ref.is_valid()) {
          emin_ch_ref.set(calo_hit);
        } else if (calo_hit.get_energy() < emin_ch_ref.get().get_energy()) {
          emin_ch_ref.set(calo_hit);
        }
        if (!emax_ch_ref.is_valid()) {
          emax_ch_ref.set(calo_hit);
        } else if (calo_hit.get_energy() > emax_ch_ref.get().get_energy()) {
          emax_ch_ref.set(calo_hit);
        }
      }

      emin_ch_ref.get_properties().store_flag("min_energy");
      emax_ch_ref.get_properties().store_flag("max_energy");
      emin_ch_ref.get().tree_dump(std::clog, "Calorimeter hit with min energy:");
      emax_ch_ref.get().tree_dump(std::clog, "Calorimeter hit with max energy:");
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
