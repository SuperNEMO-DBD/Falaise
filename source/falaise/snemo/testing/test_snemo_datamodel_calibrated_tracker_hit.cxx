// test_calibrated_tracker_hit.cxx

// Standard library
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party
// - Bayeux/datatools
#include <datatools/smart_ref.h>
#include <datatools/clhep_units.h>

// This project
#include <falaise/snemo/datamodels/calibrated_tracker_hit.h>

int main(/* int argc_, char ** argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::calibrated_tracker_hit'!" << std::endl;

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
      sdm::calibrated_tracker_hit my_gg_hit;
      my_gg_hit.set_hit_id(2125715);
      geomtools::geom_id gid(1204, 0, 1, 7, 35);
      my_gg_hit.set_geom_id(gid);
      my_gg_hit.grab_auxiliaries().store_flag("fake");
      my_gg_hit.grab_auxiliaries().store("mascot", "snaily");
      my_gg_hit.set_r(13.4 * CLHEP::mm);
      my_gg_hit.set_sigma_r(0.7 * CLHEP::mm);
      my_gg_hit.set_z(45.6 * CLHEP::cm);
      my_gg_hit.set_sigma_z(0.7 * CLHEP::cm);
      my_gg_hit.set_xy(10.0 * CLHEP::mm, 20.0 * CLHEP::mm);
      my_gg_hit.set_noisy(false);
      my_gg_hit.set_delayed(false);
      my_gg_hit.set_peripheral(false);
      my_gg_hit.set_bottom_cathode_missing(false);
      my_gg_hit.set_top_cathode_missing(false);
      my_gg_hit.tree_dump(std::clog, "Calibrated tracker hit: ");
    }

    {
      // create a list of random tracker hits:
      srand48(314159);
      typedef std::vector<sdm::calibrated_tracker_hit> hit_collection_type;
      hit_collection_type list_of_gg_hits;

      int nb_hits = 10;
      list_of_gg_hits.reserve(nb_hits);
      double cell_diameter = 44.0 * CLHEP::mm;
      double cell_radius = 0.5 * cell_diameter;
      for(int i = 0; i < nb_hits; i++) {
        // append a new empty hit in the list:
        list_of_gg_hits.push_back(sdm::calibrated_tracker_hit());
        // get a reference to the newly pushed hit:
        sdm::calibrated_tracker_hit & my_gg_hit = list_of_gg_hits.back();
        my_gg_hit.set_hit_id(2125715 + i);
        geomtools::geom_id gid(1204,               // type for the 'drift_cell_core' category
                               0,                  // module
                               1,                  // half chamber
                               (uint32_t)(drand48() * 9),     // layer
                               (uint32_t)(drand48() * 113) ); // cell
        my_gg_hit.set_geom_id(gid);
        my_gg_hit.grab_auxiliaries().store_flag("fake");
        double z_error = 1.0 * CLHEP::cm;
        if(drand48() < 0.1) {
          my_gg_hit.set_bottom_cathode_missing(true);
          z_error = 10.0 * CLHEP::cm;
        }
        if(drand48() < 0.1) {
          my_gg_hit.set_top_cathode_missing(true);
          if(z_error > 2.0 * CLHEP::cm) z_error = 150.0 * CLHEP::cm;
          else z_error = 10.0 * CLHEP::cm;
        }
        if(drand48() < 0.2) {
          my_gg_hit.set_noisy(true);
        }
        my_gg_hit.set_z(125.0 * CLHEP::cm);
        my_gg_hit.set_sigma_z(z_error);
        my_gg_hit.set_r(drand48() * cell_radius);
        if(my_gg_hit.get_r() > 0.95  * cell_radius) {
          my_gg_hit.set_peripheral(true);
        }
        my_gg_hit.set_sigma_r(1.0 * CLHEP::mm);
        // X/Y position of the anode wire(here we use fake values):
        my_gg_hit.set_xy(i * cell_diameter, i * cell_diameter);
      }

      // search for hits with min/max anode drift radius:
      typedef datatools::smart_ref<sdm::calibrated_tracker_hit> smart_ref_type;
      smart_ref_type rmin_gg_ref;
      smart_ref_type rmax_gg_ref;
      for(hit_collection_type::const_iterator i = list_of_gg_hits.begin();
          i != list_of_gg_hits.end();
          i++) {
        const sdm::calibrated_tracker_hit & gg_hit = *i;
        std::ostringstream title;
        title << "Tracker hit #" << gg_hit.get_hit_id();
        gg_hit.tree_dump(std::clog, title.str());
        if(! rmin_gg_ref.is_valid()) {
          rmin_gg_ref.set(gg_hit);
        } else if(gg_hit.get_r() < rmin_gg_ref.get().get_r()) {
          rmin_gg_ref.set(gg_hit);
        }
        if(! rmax_gg_ref.is_valid()) {
          rmax_gg_ref.set(gg_hit);
        } else if(gg_hit.get_r() > rmax_gg_ref.get().get_r()) {
          rmax_gg_ref.set(gg_hit);
        }
      }

      rmin_gg_ref.get_properties().store_flag("min_radius");
      rmax_gg_ref.get_properties().store_flag("max_radius");
      rmin_gg_ref.get().tree_dump(std::clog, "Tracker hit with min r:");
      rmax_gg_ref.get().tree_dump(std::clog, "Tracker hit with max r:");
    }

    std::clog << "The end." << std::endl;
  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}
