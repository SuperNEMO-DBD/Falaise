// test_tracker_trajectory.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Boost/datatools:
#include <datatools/clhep_units.h>

// This project:
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/polyline_trajectory_pattern.h>
#include <falaise/snemo/datamodels/tracker_cluster.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'snemo::datamodel::tracker_trajectory' class." << std::endl;

    namespace sdm = snemo::datamodel;

    srand48(314159);

    // Populate a collection of handles on Geiger hits :
    sdm::TrackerHitHdlCollection hits;
    for (int i = 0; i < 10; ++i) {
      sdm::TrackerHitHdl h(new sdm::calibrated_tracker_hit);
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
      // {
      //   std::ostringstream title;
      //   title << "Hit #" << i;
      //   gg_hit.tree_dump(std::clog, title.str());
      // }
    }

    // Create a handle on some tracker cluster :
    sdm::TrackerClusterHdl hTC0(new sdm::tracker_cluster);
    sdm::tracker_cluster& TC0 = hTC0.grab();
    TC0.set_cluster_id(0);
    TC0.make_prompt();
    TC0.hits().push_back(hits[0]);
    TC0.hits().push_back(hits[1]);
    TC0.hits().push_back(hits[2]);
    TC0.hits().push_back(hits[3]);
    TC0.hits().push_back(hits[4]);
    TC0.grab_auxiliaries().store("display.color", "blue");
    {
      std::ostringstream title;
      title << "Cluster #0";
      TC0.tree_dump(std::clog, title.str());
      std::clog << std::endl;
    }

    {
      // Create a handle on some trajectory pattern :
      datatools::handle<sdm::base_trajectory_pattern> hLTP0;
      auto* LTP = new sdm::line_trajectory_pattern;
      // access to the internal geometry model :
      LTP->get_segment().set_first(geomtools::vector_3d(3., 5., 7.));
      LTP->get_segment().set_last(geomtools::vector_3d(13., -5., 12.));
      hLTP0.reset(LTP);

      sdm::tracker_trajectory TJ0;
      TJ0.set_id(0);
      TJ0.set_cluster_handle(hTC0);
      TJ0.set_pattern_handle(hLTP0);
      TJ0.grab_auxiliaries().store_flag("test");
      TJ0.grab_auxiliaries().store("chi2", 0.234);
      TJ0.tree_dump(std::clog, "Tracker trajectory : ");
      std::clog << std::endl;
    }

    {
      /*
      // Create a handle on some trajectory pattern :
      datatools::handle<sdm::base_trajectory_pattern> hLTP1;
      sdm::polyline_trajectory_pattern * PLTP = new sdm::polyline_trajectory_pattern;
      // access to the internal geometry model :
      PLTP->grab_path().add(geomtools::vector_3d(3., 5., 7.));
      PLTP->grab_path().add(geomtools::vector_3d(5., 3., 8.5));
      PLTP->grab_path().add(geomtools::vector_3d(7., 2., 10.));
      PLTP->grab_path().add(geomtools::vector_3d(9., -1., 10.5));
      PLTP->grab_path().add(geomtools::vector_3d(13., -5., 12.));
      hLTP1.reset(PLTP);
      std::clog << "The end." << std::endl;

      sdm::tracker_trajectory TJ1;
      TJ1.set_trajectory_id(0);
      TJ1.set_cluster_handle(hTC0);
      TJ1.set_pattern_handle(hLTP1);
      TJ1.grab_auxiliaries().store_flag("test");
      TJ1.grab_auxiliaries().store("chi2", 1.234);
      TJ1.tree_dump(std::clog, "Tracker trajectory : ");
      */
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
