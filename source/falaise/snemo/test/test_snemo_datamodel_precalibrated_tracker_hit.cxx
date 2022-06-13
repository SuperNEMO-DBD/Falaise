// test_precalibrated_tracker_hit.cxx

// Standard library
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party
// - Bayeux/datatools
#include <datatools/clhep_units.h>
#include <datatools/smart_ref.h>

// This project
#include <falaise/snemo/datamodels/precalibrated_tracker_hit.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::precalibrated_tracker_hit'!" << std::endl;

    /*
      bool debug = false;

      int iarg = 1;
      while(iarg < argc_) {
      std::string token = argv_[iarg];

      if(token[0] == '-') {
      std::string option = token;
      if((option == "-d") ||(option == "--debug")) {
      debug = true;
      } else {
      std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
      }
      } else {
      std::string argument = token;
      {
      std::clog << "warning: ignoring argument '"
      << argument << "'!" << std::endl;
      }
      }
      iarg++;
      }
    */

    namespace sdm = snemo::datamodel;

    {
      sdm::precalibrated_tracker_hit my_gg_hit;
      my_gg_hit.set_hit_id(2125715);
      geomtools::geom_id gid(1204, 0, 1, 7, 35);
      my_gg_hit.set_geom_id(gid);
      my_gg_hit.grab_auxiliaries().store_flag("fake");
      my_gg_hit.grab_auxiliaries().store("mascot", "snaily");
      my_gg_hit.set_reference_time(0.7 * CLHEP::microsecond);
      my_gg_hit.set_anodic_drift_time(3.2 * CLHEP::microsecond);
      my_gg_hit.set_bottom_cathode_drift_time(18 * CLHEP::microsecond);
      my_gg_hit.set_top_cathode_drift_time(45 * CLHEP::microsecond);
      my_gg_hit.set_delayed(false);

      my_gg_hit.tree_dump(std::clog, "Precalibrated tracker hit: ");
      std::clog << "My geiger hit : Plasma propagation time = " << my_gg_hit.get_plasma_propagation_time() << std::endl;
    }  // namespace sdm=snemo::datamodel;

    {
      // create a list of random tracker hits:
      srand48(314159);
      using hit_collection_type = std::vector<sdm::precalibrated_tracker_hit>;
      hit_collection_type list_of_gg_hits;

      int nb_hits = 10;
      list_of_gg_hits.reserve(nb_hits);
      for (int i = 0; i < nb_hits; i++) {
        // append a new empty hit in the list:
        list_of_gg_hits.push_back(sdm::precalibrated_tracker_hit());
        // get a reference to the newly pushed hit:
        sdm::precalibrated_tracker_hit& my_gg_hit = list_of_gg_hits.back();
        my_gg_hit.set_hit_id(2125715 + i);
        geomtools::geom_id gid(1204,  // type for the 'drift_cell_core' category
                               0,     // module
                               1,     // half chamber
                               (uint32_t)(drand48() * 9),     // layer
                               (uint32_t)(drand48() * 113));  // cell
        my_gg_hit.set_geom_id(gid);
        my_gg_hit.grab_auxiliaries().store_flag("fake");
        if (drand48() > 0.1) {
          my_gg_hit.set_bottom_cathode_drift_time(drand48() * 18 * CLHEP::microsecond);
        }
        if (drand48() < 0.1) {
          my_gg_hit.set_top_cathode_drift_time(drand48() * 45 * CLHEP::microsecond);
        }
        if (drand48() < 0.05) {
          my_gg_hit.set_delayed(true);
        }
        my_gg_hit.set_reference_time(drand48() * 0.7 * CLHEP::microsecond);
        my_gg_hit.set_anodic_drift_time(drand48() * 3.2 * CLHEP::microsecond);
      }

      // search for hits with min/max anode drift radius:
      using smart_ref_type = datatools::smart_ref<sdm::precalibrated_tracker_hit>;
      smart_ref_type rmin_gg_ref;
      smart_ref_type rmax_gg_ref;
      for (hit_collection_type::const_iterator i = list_of_gg_hits.begin();
           i != list_of_gg_hits.end(); i++) {
        const sdm::precalibrated_tracker_hit& gg_hit = *i;
        std::ostringstream title;
        title << "Tracker hit #" << gg_hit.get_hit_id();
        gg_hit.tree_dump(std::clog, title.str());
        if (!rmin_gg_ref.is_valid()) {
          rmin_gg_ref.set(gg_hit);
        } else if (gg_hit.get_anodic_drift_time() < rmin_gg_ref.get().get_anodic_drift_time()) {
          rmin_gg_ref.set(gg_hit);
        }
        if (!rmax_gg_ref.is_valid()) {
          rmax_gg_ref.set(gg_hit);
        } else if (gg_hit.get_anodic_drift_time() > rmax_gg_ref.get().get_anodic_drift_time()) {
          rmax_gg_ref.set(gg_hit);
        }
      }

      rmin_gg_ref.get_properties().store_flag("min_radius");
      rmax_gg_ref.get_properties().store_flag("max_radius");
      rmin_gg_ref.get().tree_dump(std::clog, "Tracker hit with min anodic drift time:");
      rmax_gg_ref.get().tree_dump(std::clog, "Tracker hit with max anodic drift time:");
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
