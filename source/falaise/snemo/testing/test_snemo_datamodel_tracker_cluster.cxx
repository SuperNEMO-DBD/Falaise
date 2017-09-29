// test_snemo_datamodel_tracker_cluster.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Boost/datatools:
#include <datatools/clhep_units.h>

// This project:
#include <falaise/snemo/datamodels/tracker_cluster.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'tracker_cluster' class." << std::endl;

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
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }
    */

    namespace sdm = snemo::datamodel;

    srand48(314159);

    // Populate a collection of handles on Geiger hits :
    sdm::calibrated_tracker_hit::collection_type hits;
    for (int i = 0; i < 10; ++i) {
      sdm::calibrated_tracker_hit::handle_type h(new sdm::calibrated_tracker_hit);
      sdm::calibrated_tracker_hit& gg_hit = h.grab();
      gg_hit.set_hit_id(i);
      geomtools::geom_id gid;
      gid.set_type(1204);
      gid.set_address(0, 0, i, 45 + i / 2);
      gg_hit.set_geom_id(gid);
      gg_hit.grab_auxiliaries().store_flag("fake");
      gg_hit.set_r(22. * CLHEP::mm * drand48());
      gg_hit.set_sigma_r(1. * CLHEP::mm);
      gg_hit.set_z((50. + i) * CLHEP::cm + (-1. + 2 * drand48()) * CLHEP::cm);
      gg_hit.set_sigma_z(0.5 * CLHEP::cm);
      hits.push_back(h);
      {
        std::ostringstream title;
        title << "Hit #" << i;
        gg_hit.tree_dump(std::clog, title.str());
      }
    }

    // Create a handle on some tracker cluster :
    sdm::tracker_cluster::handle_type hTC1(new sdm::tracker_cluster);
    sdm::tracker_cluster& TC1 = hTC1.grab();
    TC1.set_cluster_id(0);
    TC1.make_prompt();
    TC1.grab_hits().push_back(hits[0]);
    TC1.grab_hits().push_back(hits[1]);
    TC1.grab_hits().push_back(hits[2]);
    TC1.grab_hits().push_back(hits[3]);
    TC1.grab_hits().push_back(hits[4]);
    TC1.grab_auxiliaries().store("display.color", "blue");
    {
      std::ostringstream title;
      title << "Cluster #0";
      TC1.tree_dump(std::clog, title.str());
    }

    // Create another handle on some othertracker cluster :
    sdm::tracker_cluster::handle_type hTC2(new sdm::tracker_cluster);
    sdm::tracker_cluster& TC2 = hTC2.grab();
    TC2.set_cluster_id(1);
    TC2.make_prompt();
    TC2.grab_hits().push_back(hits[6]);
    TC2.grab_hits().push_back(hits[7]);
    TC2.grab_hits().push_back(hits[8]);
    TC2.grab_hits().push_back(hits[9]);
    TC2.grab_auxiliaries().store("display.color", "red");
    {
      std::ostringstream title;
      title << "Cluster #1";
      TC2.tree_dump(std::clog, title.str());
    }

    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
