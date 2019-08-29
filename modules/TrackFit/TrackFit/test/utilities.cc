// Ourselves
#include <utilities.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/temporary_files.h>
// - Bayeux/geomtools:
#include <geomtools/color.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/manager.h>

// Falaise:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>

void generate_tcd(const snemo::geometry::gg_locator &ggloc_,
                  snemo::datamodel::calibrated_data::tracker_hit_collection_type &gghits_,
                  snemo::datamodel::tracker_clustering_data &tcd_) {
  snemo::datamodel::calibrated_data::tracker_hit_collection_type &gghits = gghits_;

  // A collection of fake Geiger hits:
  for (int i = 0; i < 9; i++) {
    snemo::datamodel::calibrated_data::tracker_hit_handle_type hgghit(
        new snemo::datamodel::calibrated_tracker_hit);
    snemo::datamodel::calibrated_tracker_hit &gghit = hgghit.grab();
    gghit.set_hit_id(i);
    gghit.grab_auxiliaries().store_flag("mock");
    gghit.grab_auxiliaries().store("cluster_id", 0);
    gghit.grab_geom_id().set_type(1204);
    gghit.grab_geom_id().set_address(0, 1, i, 44 + i);
    gghit.set_z((25.0 + 1.2 * drand48()) * CLHEP::cm);
    // gghit.set_r((0.1 + 2.1 * drand48()) * CLHEP::cm);
    gghit.set_sigma_z(0.7 * CLHEP::cm);
    gghit.set_r(2.0 * CLHEP::cm);
    if (i == 0) {
      gghit.set_r(1.8 * CLHEP::cm);
    } else if (i == 1) {
      gghit.set_r(1.9 * CLHEP::cm);
    } else if (i == 2) {
      gghit.grab_geom_id().set_address(0, 1, i, 44 + i - 1);
      gghit.set_r(1.2 * CLHEP::cm);
    } else if (i == 7) {
      gghit.set_r(1.9 * CLHEP::cm);
    } else if (i == 8) {
      gghit.set_r(1.8 * CLHEP::cm);
    }
    gghit.set_sigma_r(0.3 * CLHEP::mm);
    gghit.set_delayed(false);
    gghit.set_bottom_cathode_missing(false);
    gghit.set_top_cathode_missing(false);
    geomtools::vector_3d cell_position = ggloc_.getCellPosition(gghit.get_geom_id());
    gghit.set_xy(cell_position.x(), cell_position.y());
    // gghit.tree_dump(std::clog, "Generated Geiger hit: ");
    gghits.push_back(hgghit);
  }

  // A fake clustering solution:
  snemo::datamodel::TrackerClusteringSolutionHdl htcs(
      new snemo::datamodel::tracker_clustering_solution);
  snemo::datamodel::tracker_clustering_solution &tcs = htcs.grab();
  tcs.set_solution_id(0);
  tcs.get_auxiliaries().store_flag("mock");

  // Add a cluster:
  snemo::datamodel::TrackerClusterHdl htc0(new snemo::datamodel::tracker_cluster);
  snemo::datamodel::tracker_cluster &tc0 = htc0.grab();
  htc0.grab().grab_auxiliaries().store_flag("mock");
  htc0.grab().set_cluster_id(0);
  for (int i = 0; i < (int)gghits.size(); i++) {
    snemo::datamodel::calibrated_data::tracker_hit_handle_type &gghit = gghits[i];
    tc0.get_hits().push_back(gghit);
  }

  // Another fake track:
  for (int i = 0; i < 6; i++) {
    snemo::datamodel::calibrated_data::tracker_hit_handle_type hgghit(
        new snemo::datamodel::calibrated_tracker_hit);
    snemo::datamodel::calibrated_tracker_hit &gghit = hgghit.grab();
    gghit.set_hit_id(i);
    gghit.grab_auxiliaries().store_flag("mock");
    gghit.grab_auxiliaries().store("cluster_id", 1);
    gghit.grab_geom_id().set_type(1204);
    gghit.grab_geom_id().set_address(0, 1, i, 42 - i);
    gghit.set_z((27.0 + 2.5 * i) * CLHEP::cm);
    gghit.set_sigma_z(0.7 * CLHEP::cm);
    gghit.set_r((0.1 + 2.1 * drand48()) * CLHEP::cm);
    gghit.set_r(2.2 * CLHEP::cm);
    gghit.set_sigma_r(0.3 * CLHEP::mm);
    gghit.set_delayed(false);
    // gghit.set_delayed(true);
    // gghit.set_delayed_time(234.0 * CLHEP::microsecond);
    gghit.set_bottom_cathode_missing(false);
    gghit.set_top_cathode_missing(false);
    geomtools::vector_3d cell_position = ggloc_.getCellPosition(gghit.get_geom_id());
    gghit.set_xy(cell_position.x(), cell_position.y());
    // gghit.tree_dump(std::clog, "Generated geiger hit: ");
    gghits.push_back(hgghit);
  }

  // Add another cluster:
  snemo::datamodel::TrackerClusterHdl htc1(new snemo::datamodel::tracker_cluster);
  snemo::datamodel::tracker_cluster &tc1 = htc1.grab();
  htc1.grab().grab_auxiliaries().store_flag("mock");
  htc1.grab().set_cluster_id(1);
  for (int i = 0; i < (int)gghits.size(); i++) {
    snemo::datamodel::calibrated_data::tracker_hit_handle_type &gghit = gghits[i];
    if (gghit->get_auxiliaries().has_key("cluster_id")) {
      if (gghit->get_auxiliaries().fetch_integer("cluster_id") == 1) {
        tc1.get_hits().push_back(gghit);
      }
    }
  }

  // Yet another fake track:
  for (int i = 0; i < 7; i++) {
    snemo::datamodel::calibrated_data::tracker_hit_handle_type hgghit(
        new snemo::datamodel::calibrated_tracker_hit);
    snemo::datamodel::calibrated_tracker_hit &gghit = hgghit.grab();
    gghit.set_hit_id(i);
    gghit.grab_auxiliaries().store_flag("mock");
    gghit.grab_auxiliaries().store("cluster_id", 2);
    gghit.grab_geom_id().set_type(1204);
    int layer = i;
    double r = 2.1 * CLHEP::cm;
    if (i == 0) {
      layer = 1;
      r = 0.5 * CLHEP::cm;
    } else if (i == 1) {
      layer = 2;
      r = 0.7 * CLHEP::cm;
    } else if (i == 2) {
      layer = 2;
      r = 1.4 * CLHEP::cm;
    } else if (i == 3) {
      layer = 3;
      r = 0.8 * CLHEP::cm;
    } else if (i == 4) {
      layer = 3;
      r = 0.7 * CLHEP::cm;
    } else if (i == 5) {
      layer = 3;
      r = 2.0 * CLHEP::cm;
    } else if (i == 6) {
      layer = 4;
      r = 1.3 * CLHEP::cm;
    }
    gghit.grab_geom_id().set_address(0, 1, layer, 49 + i);
    gghit.set_z((27.0 + 2.5 * i) * CLHEP::cm);
    gghit.set_sigma_z(0.7 * CLHEP::cm);
    gghit.set_r(r);
    gghit.set_sigma_r(0.3 * CLHEP::mm);
    gghit.set_delayed(false);
    gghit.set_bottom_cathode_missing(false);
    gghit.set_top_cathode_missing(false);
    geomtools::vector_3d cell_position = ggloc_.getCellPosition(gghit.get_geom_id());
    gghit.set_xy(cell_position.x(), cell_position.y());
    // gghit.tree_dump(std::clog, "Generated geiger hit: ");
    gghits.push_back(hgghit);
  }

  // Yet another cluster:
  snemo::datamodel::TrackerClusterHdl htc2(new snemo::datamodel::tracker_cluster);
  snemo::datamodel::tracker_cluster &tc2 = htc2.grab();
  htc2->grab_auxiliaries().store_flag("mock");
  htc2->set_cluster_id(2);
  for (int i = 0; i < (int)gghits.size(); i++) {
    snemo::datamodel::calibrated_data::tracker_hit_handle_type &gghit = gghits[i];
    if (gghit->get_auxiliaries().has_key("cluster_id")) {
      if (gghit->get_auxiliaries().fetch_integer("cluster_id") == 2) {
        tc2.get_hits().push_back(gghit);
      }
    }
  }

  // Add random fake hits
  for (int i = 0; i < 3; i++) {
    snemo::datamodel::calibrated_data::tracker_hit_handle_type hgghit(
        new snemo::datamodel::calibrated_tracker_hit);
    snemo::datamodel::calibrated_tracker_hit &gghit = hgghit.grab();
    gghit.set_hit_id(i);
    gghit.grab_auxiliaries().store_flag("mock");
    gghit.grab_auxiliaries().store("cluster_id", -1);
    gghit.grab_geom_id().set_type(1204);
    gghit.grab_geom_id().set_address(0, 1, (int)(9 * drand48()), 58 + (int)(6 * drand48()));
    gghit.set_z((37.0 + 10.2 * drand48()) * CLHEP::cm);
    gghit.set_sigma_z(0.7 * CLHEP::cm);
    gghit.set_r((0.1 + 2.1 * drand48()) * CLHEP::cm);
    gghit.set_sigma_r(0.3 * CLHEP::mm);
    gghit.set_delayed(false);
    gghit.set_bottom_cathode_missing(false);
    gghit.set_top_cathode_missing(false);
    geomtools::vector_3d cell_position = ggloc_.getCellPosition(gghit.get_geom_id());
    gghit.set_xy(cell_position.x(), cell_position.y());
    // gghit.tree_dump(std::clog, "Generated geiger hit: ");
    gghits.push_back(hgghit);
    tcs.get_unclustered_hits().push_back(hgghit);
  }

  tcs.get_clusters().push_back(htc0);
  tcs.get_clusters().push_back(htc1);
  tcs.get_clusters().push_back(htc2);
  tcd_.add_solution(htcs);
  tcs.tree_dump(std::cerr, "Tracker clustering solution:", "", true);
  std::cerr << "|-- "
            << "Tracker cluster 0: " << std::endl;
  tc0.tree_dump(std::cerr, "", "|  ");
  std::cerr << "|-- "
            << "Tracker cluster 1: " << std::endl;
  tc1.tree_dump(std::cerr, "", "|  ");
  std::cerr << "`-- "
            << "Tracker cluster 2: " << std::endl;
  tc2.tree_dump(std::cerr, "", "   ");

  return;
}

void display_event(const snemo::geometry::gg_locator &ggloc_,
                   const snemo::datamodel::calibrated_data::tracker_hit_collection_type &gghits_,
                   const snemo::datamodel::tracker_clustering_data &tcd_,
                   const snemo::datamodel::tracker_trajectory_data &ttd_) {
  namespace sdm = snemo::datamodel;
  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", ".yaca_drawer_");

  const sdm::calibrated_data::tracker_hit_collection_type &gghits = gghits_;

  // No rotation
  geomtools::rotation_3d identity;

  // Bounding box:
  geomtools::gnuplot_draw::xyz_range &BB = geomtools::gnuplot_draw::bounding_box();
  BB.reset_ranges();
  BB.activate();

  // Color context:
  geomtools::color::context &CC = geomtools::gnuplot_draw::color_context();

  // Draw hits:
  for (int i = 0; i < (int)gghits.size(); i++) {
    const sdm::calibrated_data::tracker_hit_handle_type &hgghit = gghits[i];
    const sdm::calibrated_tracker_hit &gghit = hgghit.get();
    double x = gghit.get_x();
    double y = gghit.get_y();
    double z = gghit.get_z();
    double ez = gghit.get_sigma_z();
    double r = gghit.get_r();
    double er = gghit.get_sigma_r();
    geomtools::vector_3d hit_pos(x, y, z);
    CC.set_color_code(geomtools::color::COLOR_BLUE);
    geomtools::gnuplot_draw::draw_circle(tmp_file.out(), hit_pos, identity, r + er);
    geomtools::gnuplot_draw::draw_circle(tmp_file.out(), hit_pos, identity, r - er);
    geomtools::gnuplot_draw::draw_line(tmp_file.out(), hit_pos + geomtools::vector_3d(0, 0, +ez),
                                       hit_pos + geomtools::vector_3d(0, 0, -ez));
  }

  // Draw clusters:
  if (tcd_.has_default_solution()) {
    const sdm::tracker_clustering_solution &tcd_sol = tcd_.get_default_solution();
    tcd_sol.tree_dump(std::cerr, "Clustering solution: ", "DEVEL: ");
    for (int i = 0; i < (int)tcd_sol.get_clusters().size(); i++) {
      const sdm::TrackerClusterHdl &hcluster = tcd_sol.get_clusters()[i];
      const sdm::tracker_cluster &cluster = hcluster.get();
      // cluster.tree_dump(std::cerr, "Cluster: ", "DEVEL: ");
      const sdm::TrackerHitHdlCollection &clhits = cluster.get_hits();
      if (i == 0) CC.set_color_code(geomtools::color::COLOR_RED);
      if (i == 1) CC.set_color_code(geomtools::color::COLOR_GREEN);
      if (i == 2) CC.set_color_code(geomtools::color::COLOR_MAGENTA);
      if (i == 3) CC.set_color_code(geomtools::color::COLOR_YELLOW);
      if (i == 4) CC.set_color_code(geomtools::color::COLOR_ORANGE);
      if (i > 4) CC.set_color_code(geomtools::color::COLOR_BLUE);
      for (int j = 0; j < (int)clhits.size(); j++) {
        const sdm::calibrated_data::tracker_hit_handle_type &hclhit = clhits[j];
        const sdm::calibrated_tracker_hit &clhit = hclhit.get();
        double x = clhit.get_x();
        double y = clhit.get_y();
        double z = clhit.get_z();
        double ez = clhit.get_sigma_z();
        // double r  = clhit.get_r();
        // double er = clhit.get_sigma_r();
        geomtools::vector_3d hit_pos(x, y, z);
        geomtools::gnuplot_draw::draw_box(tmp_file.out(), hit_pos, identity, ggloc_.cellDiameter(),
                                          ggloc_.cellDiameter(), 2 * ez);
      }
    }
    CC.set_color_code(geomtools::color::COLOR_BLACK);
    for (int j = 0; j < (int)tcd_sol.get_unclustered_hits().size(); j++) {
      const sdm::calibrated_data::tracker_hit_handle_type &huclhit =
          tcd_sol.get_unclustered_hits()[j];
      const sdm::calibrated_tracker_hit &uclhit = huclhit.get();
      double x = uclhit.get_x();
      double y = uclhit.get_y();
      double z = uclhit.get_z();
      double ez = uclhit.get_sigma_z();
      // double r  = uclhit.get_r();
      // double er = uclhit.get_sigma_r();
      geomtools::vector_3d hit_pos(x, y, z);
      geomtools::gnuplot_draw::draw_box(tmp_file.out(), hit_pos, identity, ggloc_.cellDiameter(),
                                        ggloc_.cellDiameter(), 2 * ez);
    }
  }

  // Draw trajectories:
  if (ttd_.has_default_solution()) {
    double chi2r_max = 10.0;
    const sdm::tracker_trajectory_solution &ttd_sol = ttd_.get_default_solution();
    const sdm::TrackerTrajectoryHdlCollection &trajectories = ttd_sol.get_trajectories();
    for (int i = 0; i < (int)trajectories.size(); i++) {
      const sdm::TrackerTrajectoryHdl &htraj = trajectories[i];
      const sdm::tracker_trajectory &traj = htraj.get();
      double chi2 = traj.get_auxiliaries().fetch_real("chi2");
      size_t ndof = traj.get_auxiliaries().fetch_integer("ndof");
      double chi2r = chi2 / ndof;
      if (traj.has_pattern() && chi2r < chi2r_max) {
        const sdm::base_trajectory_pattern &traj_pattern = traj.get_pattern();
        const std::string &traj_pattern_id = traj_pattern.get_pattern_id();
        if (traj_pattern_id == sdm::line_trajectory_pattern::pattern_id()) {
          const sdm::line_trajectory_pattern &line_pattern =
              dynamic_cast<const sdm::line_trajectory_pattern &>(traj_pattern);
          const geomtools::line_3d &segment = line_pattern.get_segment();
          geomtools::vector_3d dummy_pos(0, 0, 0);
          CC.set_color_code(geomtools::color::COLOR_BLUE);
          geomtools::gnuplot_draw::draw_line(tmp_file.out(), dummy_pos, identity, segment);

        } else if (traj_pattern_id == sdm::helix_trajectory_pattern::pattern_id()) {
          const sdm::helix_trajectory_pattern &helix_pattern =
              dynamic_cast<const sdm::helix_trajectory_pattern &>(traj_pattern);
          const geomtools::helix_3d &helix = helix_pattern.get_helix();
          geomtools::vector_3d dummy_pos(0, 0, 0);
          CC.set_color_code(geomtools::color::COLOR_RED);
          geomtools::gnuplot_draw::draw_helix(tmp_file.out(), dummy_pos, identity, helix);
        }
      }
    }
  }

  Gnuplot g1("lines");
  g1.cmd("set grid");
  g1.cmd("set size ratio -1");
  g1.cmd("set view equal xyz");

  std::ostringstream gp_cmd;
  gp_cmd << "splot ";
  gp_cmd << "'" << tmp_file.get_filename() << "' "
         << "index 0 using 1:2:3:4 notitle with lines linecolor variable lw 1 ";

  g1.cmd(gp_cmd.str());
  g1.showonscreen();  // window output
  geomtools::gnuplot_drawer::wait_for_key();
  usleep(200);

  return;
}
