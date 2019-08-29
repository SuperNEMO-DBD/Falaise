// test_tracker_trajectory_solution.cxx

// Standard library
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/handle.h>
#include <datatools/handle_macros.h>
#include <datatools/io_factory.h>

// For Boost I/O :
// Some pre-processor guard about Boost I/O usage and linkage :
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/tracker_cluster.h>
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>
#include <falaise/snemo/datamodels/tracker_trajectory_solution.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'snemo::datamodel::tracker_trajectory_solution' class."
              << std::endl;

    namespace sdm = snemo::datamodel;

    srand48(314159);

    /*************************
     *                       *
     *        H I T S        *
     *                       *
     *************************/

    // Populate a collection of handles on Geiger hits :
    sdm::TrackerHitHdlCollection hits;
    for (int i = 0; i < 18; ++i) {
      DATATOOLS_HANDLE_DECLARE_NEW(hgg_hit, sdm::calibrated_tracker_hit);
      DATATOOLS_HANDLE_GRAB_REF(gg_hit, hgg_hit, sdm::calibrated_tracker_hit);
      // calibrated_tracker_hit::handle_type h(new sdm::calibrated_tracker_hit);
      // calibrated_tracker_hit & gg_hit = h.get();
      gg_hit.set_hit_id(i);
      geomtools::geom_id gid;
      gid.set_type(1204);
      if (i < 10) {
        gid.set_address(0, 0, i, 45 + i / 2);
      } else if (i < 16) {
        gid.set_address(0, 1, (i - 10) / 2, 42 - (i - 10));
      } else if (i == 16) {
        gid.set_address(0, 1, 4, 45);
      } else if (i == 17) {
        gid.set_address(0, 1, 6, 48);
      }
      gg_hit.set_geom_id(gid);
      gg_hit.grab_auxiliaries().store_flag("fake");
      gg_hit.set_r(22. * CLHEP::mm * drand48());
      gg_hit.set_sigma_r(1. * CLHEP::mm);
      gg_hit.set_z((50. + i) * CLHEP::cm + (-1. + 2 * drand48()) * CLHEP::cm);
      gg_hit.set_sigma_z(0.5 * CLHEP::cm);
      hits.push_back(hgg_hit);
    }

    /*************************
     *                       *
     *    C L U S T E R S    *
     *                       *
     *************************/

    // Create a handle on some tracker cluster :
    DATATOOLS_HANDLE_DECLARE_NEW(hTC0, sdm::tracker_cluster);
    DATATOOLS_HANDLE_GRAB_REF(TC0, hTC0, sdm::tracker_cluster);
    // tracker_cluster::handle_type hTC0(new sdm::tracker_cluster);
    // tracker_cluster & TC0 = hTC0.get();
    TC0.set_cluster_id(0);
    TC0.make_prompt();
    TC0.get_hits().push_back(hits.at(0));
    TC0.get_hits().push_back(hits.at(1));
    TC0.get_hits().push_back(hits.at(2));
    TC0.get_hits().push_back(hits.at(3));
    TC0.get_hits().push_back(hits.at(4));
    TC0.grab_auxiliaries().store("display.color", "blue");
    {
      std::ostringstream title;
      title << "Tracker cluster #0";
      TC0.tree_dump(std::clog, title.str());
    }

    // Create another handle on some other tracker cluster :
    DATATOOLS_HANDLE_DECLARE_NEW(hTC1, sdm::tracker_cluster);
    DATATOOLS_HANDLE_GRAB_REF(TC1, hTC1, sdm::tracker_cluster);
    // tracker_cluster::handle_type hTC1(new sdm::tracker_cluster);
    // tracker_cluster & TC1 = hTC1.get();
    TC1.set_cluster_id(1);
    TC1.make_prompt();
    TC1.get_hits().push_back(hits.at(10));
    TC1.get_hits().push_back(hits.at(11));
    TC1.get_hits().push_back(hits.at(12));
    TC1.get_hits().push_back(hits.at(13));
    TC1.get_hits().push_back(hits.at(14));
    TC1.get_hits().push_back(hits.at(15));
    TC1.grab_auxiliaries().store("display.color", "red");
    {
      std::ostringstream title;
      title << "Tracker cluster #1";
      TC1.tree_dump(std::clog, title.str());
    }

    /*************************
     *                       *
     *   CLUSTER SOLUTION    *
     *                       *
     *************************/

    DATATOOLS_HANDLE_DECLARE_NEW(hTCS0, sdm::tracker_clustering_solution);
    DATATOOLS_HANDLE_GRAB_REF(TCS0, hTCS0, sdm::tracker_clustering_solution);
    // datatools::utils::handle<sdm::tracker_clustering_solution> hTCS0(new
    // sdm::tracker_clustering_solution);  sdm::tracker_clustering_solution & TCS0 = hTCS0.get();
    TCS0.set_solution_id(0);
    TCS0.get_auxiliaries().store("weighting.chi2", 3.2546);
    TCS0.get_auxiliaries().store("weighting.ndof", 5);
    TCS0.get_unclustered_hits().push_back(hits.at(16));
    TCS0.get_unclustered_hits().push_back(hits.at(17));
    TCS0.get_clusters().push_back(hTC0);
    TCS0.get_clusters().push_back(hTC1);
    TCS0.tree_dump(std::clog, "TCS0 : ");

    /*****************************
     *                           *
     *  T R A J E C T O R I E S  *
     *                           *
     *****************************/

    // Create a handle on some trajectory pattern :
    DATATOOLS_HANDLE_DECLARE(hLTP0, sdm::base_trajectory_pattern);
    // datatools::utils::handle<sdm::base_trajectory_pattern> hLTP0;
    auto* LTP = new sdm::line_trajectory_pattern;
    // access to the internal line model :
    LTP->get_segment().set_first(geomtools::vector_3d(3., 5., 7.));
    LTP->get_segment().set_last(geomtools::vector_3d(13., -5., 12.));
    hLTP0.reset(LTP);

    DATATOOLS_HANDLE_DECLARE_NEW(hTJ0, sdm::tracker_trajectory);
    DATATOOLS_HANDLE_GRAB_REF(TJ0, hTJ0, sdm::tracker_trajectory);
    // datatools::utils::handle<sdm::tracker_trajectory> hTJ0(new sdm::tracker_trajectory);
    // tracker_trajectory & TJ0 = hTJ0.get();
    TJ0.set_trajectory_id(0);
    TJ0.set_cluster_handle(hTC0);
    TJ0.set_pattern_handle(hLTP0);
    TJ0.grab_auxiliaries().store_flag("line");
    TJ0.grab_auxiliaries().store("chi2", 0.234);
    TJ0.tree_dump(std::clog, "Tracker trajectory #0: ");

    // Create a handle on some trajectory pattern :
    DATATOOLS_HANDLE_DECLARE(hLTP1, sdm::base_trajectory_pattern);
    // datatools::utils::handle<sdm::base_trajectory_pattern> hLTP1;
    auto* HTP = new sdm::helix_trajectory_pattern;
    // access to the internal helix model :
    HTP->get_helix().set_radius(0.75 * CLHEP::m);
    HTP->get_helix().set_center(
        geomtools::vector_3d(1.3 * CLHEP::m, -0.5 * CLHEP::m, 1.2 * CLHEP::m));
    HTP->get_helix().set_step(0.10 * CLHEP::m);
    HTP->get_helix().set_angle1(0.25 * M_PI * CLHEP::radian);
    HTP->get_helix().set_angle2(0.65 * M_PI * CLHEP::radian);
    hLTP1.reset(HTP);

    DATATOOLS_HANDLE_DECLARE_NEW(hTJ1, sdm::tracker_trajectory);
    DATATOOLS_HANDLE_GRAB_REF(TJ1, hTJ1, sdm::tracker_trajectory);
    // datatools::utils::handle<tracker_trajectory> hTJ1(new sdm::tracker_trajectory);
    // tracker_trajectory & TJ1 = hTJ1.get();
    TJ1.set_trajectory_id(0);
    TJ1.set_cluster_handle(hTC1);
    TJ1.set_pattern_handle(hLTP1);
    TJ1.grab_auxiliaries().store_flag("helix");
    TJ1.grab_auxiliaries().store("chi2", 1.74);
    TJ1.tree_dump(std::clog, "Tracker trajectory #1: ");

    /*************************
     *                       *
     *  TRAJECTORY SOLUTION  *
     *                       *
     *************************/
    std::clog << std::endl;
    sdm::tracker_trajectory_solution TJS0;
    TJS0.set_solution_id(0);
    TJS0.set_clustering_solution(hTCS0);
    TJS0.grab_trajectories().push_back(hTJ0);
    TJS0.grab_trajectories().push_back(hTJ1);
    TJS0.tree_dump(std::clog, "Tracker trajectory solution #0 : ");

    std::clog << "Store..." << std::endl;
    {
      datatools::data_writer sink("TTTS.xml", datatools::using_multi_archives);
      sink.store(TJS0);
    }

    std::clog << "Load..." << std::endl;
    {
      datatools::data_reader source("TTTS.xml", datatools::using_multi_archives);
      sdm::tracker_trajectory_solution TJS_reload;
      source.load(TJS_reload);
      TJS_reload.tree_dump(std::clog, "Tracker trajectory solution(reload) : ");
    }
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
