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

void generate_gg_hits(const snemo::geometry::gg_locator& ggloc_,
                      snemo::datamodel::TrackerHitHdlCollection& gghits_) {
  // A fake track:
  for (int i = 0; i < 9; i++) {
    snemo::datamodel::TrackerHitHdl hgghit(new snemo::datamodel::calibrated_tracker_hit);
    snemo::datamodel::calibrated_tracker_hit& gghit = hgghit.grab();
    gghit.set_hit_id(i);
    gghit.grab_geom_id().set_type(1204);
    gghit.grab_geom_id().set_address(0, 1, i, 44 + i);
    gghit.set_z((25.0 + 1.2 * drand48()) * CLHEP::cm);
    gghit.set_sigma_z(0.7 * CLHEP::cm);
    gghit.set_r((0.1 + 2.1 * drand48()) * CLHEP::cm);
    gghit.set_r(2.2 * CLHEP::cm);
    gghit.set_sigma_r(0.3 * CLHEP::mm);
    gghit.set_delayed(false);
    gghit.set_bottom_cathode_missing(false);
    gghit.set_top_cathode_missing(false);
    geomtools::vector_3d cell_position = ggloc_.getCellPosition(gghit.get_geom_id());
    gghit.set_xy(cell_position.x(), cell_position.y());
    // gghit.tree_dump(std::clog, "Generated geiger hit: ");
    gghits_.push_back(hgghit);
  }

  // A fake track:
  for (int i = 0; i < 4; i++) {
    snemo::datamodel::TrackerHitHdl hgghit(new snemo::datamodel::calibrated_tracker_hit);
    snemo::datamodel::calibrated_tracker_hit& gghit = hgghit.grab();
    gghit.set_hit_id(i);
    gghit.grab_geom_id().set_type(1204);
    gghit.grab_geom_id().set_address(0, 1, i, 51);
    gghit.set_z((25.0 - 1.8 * i + 3.2 * drand48()) * CLHEP::cm);
    gghit.set_sigma_z(1.3 * CLHEP::cm);
    gghit.set_r((0.1 + 2.1 * drand48()) * CLHEP::cm);
    gghit.set_r(2.2 * CLHEP::cm);
    gghit.set_sigma_r(0.3 * CLHEP::mm);
    gghit.set_delayed(false);
    gghit.set_bottom_cathode_missing(false);
    gghit.set_top_cathode_missing(false);
    geomtools::vector_3d cell_position = ggloc_.getCellPosition(gghit.get_geom_id());
    gghit.set_xy(cell_position.x(), cell_position.y());
    // gghit.tree_dump(std::clog, "Generated geiger hit: ");
    gghits_.push_back(hgghit);
  }

  // A fake track:
  for (int i = 0; i < 3; i++) {
    snemo::datamodel::TrackerHitHdl hgghit(new snemo::datamodel::calibrated_tracker_hit);
    snemo::datamodel::calibrated_tracker_hit& gghit = hgghit.grab();
    gghit.set_hit_id(i);
    gghit.grab_geom_id().set_type(1204);
    gghit.grab_geom_id().set_address(0, 1, 5 - i, 52 + i);
    gghit.set_z((17.0 + 1.3 * i + 1.7 * drand48()) * CLHEP::cm);
    gghit.set_sigma_z(1.3 * CLHEP::cm);
    gghit.set_r((0.1 + 2.1 * drand48()) * CLHEP::cm);
    gghit.set_r(2.2 * CLHEP::cm);
    gghit.set_sigma_r(0.3 * CLHEP::mm);
    gghit.set_delayed(false);
    gghit.set_bottom_cathode_missing(false);
    gghit.set_top_cathode_missing(false);
    geomtools::vector_3d cell_position = ggloc_.getCellPosition(gghit.get_geom_id());
    gghit.set_xy(cell_position.x(), cell_position.y());
    // gghit.tree_dump(std::clog, "Generated geiger hit: ");
    gghits_.push_back(hgghit);
  }

  // A fake track:
  for (int i = 0; i < 6; i++) {
    snemo::datamodel::TrackerHitHdl hgghit(new snemo::datamodel::calibrated_tracker_hit);
    snemo::datamodel::calibrated_tracker_hit& gghit = hgghit.grab();
    gghit.set_hit_id(i);
    gghit.grab_geom_id().set_type(1204);
    gghit.grab_geom_id().set_address(0, 1, i, 42 - i);
    gghit.set_z((27.0 + 2.5 * i * drand48()) * CLHEP::cm);
    gghit.set_sigma_z(0.7 * CLHEP::cm);
    gghit.set_r((0.1 + 2.1 * drand48()) * CLHEP::cm);
    gghit.set_r(2.2 * CLHEP::cm);
    gghit.set_sigma_r(0.3 * CLHEP::mm);
    gghit.set_delayed(false);
    gghit.set_bottom_cathode_missing(false);
    gghit.set_top_cathode_missing(false);
    geomtools::vector_3d cell_position = ggloc_.getCellPosition(gghit.get_geom_id());
    gghit.set_xy(cell_position.x(), cell_position.y());
    // gghit.tree_dump(std::clog, "Generated geiger hit: ");
    gghits_.push_back(hgghit);
  }

  // Add random fake hits
  for (int i = 0; i < 3; i++) {
    snemo::datamodel::TrackerHitHdl hgghit(new snemo::datamodel::calibrated_tracker_hit);
    snemo::datamodel::calibrated_tracker_hit& gghit = hgghit.grab();
    gghit.set_hit_id(i);
    gghit.grab_geom_id().set_type(1204);
    gghit.grab_geom_id().set_address(0, 1, (int)(9 * drand48()), 56 + (int)(6 * drand48()));
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
    gghits_.push_back(hgghit);
  }

  return;
}

void display_event(const snemo::geometry::gg_locator& ggloc_,
                   const snemo::datamodel::TrackerHitHdlCollection& gghits_,
                   const snemo::datamodel::tracker_clustering_data& tcd_) {
  namespace sdm = snemo::datamodel;
  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", ".yaca_drawer_");

  geomtools::rotation_3d identity;

  // Bounding box:
  geomtools::gnuplot_draw::xyz_range& BB = geomtools::gnuplot_draw::bounding_box();
  BB.reset_ranges();
  BB.activate();

  // Color context:
  geomtools::color::context& CC = geomtools::gnuplot_draw::color_context();

  for (int i = 0; i < (int)gghits_.size(); i++) {
    const sdm::TrackerHitHdl& hgghit = gghits_[i];
    const sdm::calibrated_tracker_hit& gghit = hgghit.get();
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

  if (tcd_.has_default_solution()) {
    const sdm::tracker_clustering_solution& tcd_sol = tcd_.get_default_solution();
    tcd_sol.tree_dump(std::cerr, "Clustering solution: ", "DEVEL: ");
    for (int i = 0; i < (int)tcd_sol.get_clusters().size(); i++) {
      const sdm::TrackerClusterHdl& hcluster = tcd_sol.get_clusters()[i];
      const sdm::tracker_cluster& cluster = hcluster.get();
      // cluster.tree_dump(std::cerr, "Cluster: ", "DEVEL: ");
      const sdm::TrackerHitHdlCollection& clhits = cluster.get_hits();
      if (i == 0) CC.set_color_code(geomtools::color::COLOR_RED);
      if (i == 1) CC.set_color_code(geomtools::color::COLOR_GREEN);
      if (i == 2) CC.set_color_code(geomtools::color::COLOR_MAGENTA);
      if (i == 3) CC.set_color_code(geomtools::color::COLOR_YELLOW);
      if (i == 4) CC.set_color_code(geomtools::color::COLOR_ORANGE);
      if (i > 4) CC.set_color_code(geomtools::color::COLOR_BLUE);
      for (int j = 0; j < (int)clhits.size(); j++) {
        const sdm::TrackerHitHdl& hclhit = clhits[j];
        const sdm::calibrated_tracker_hit& clhit = hclhit.get();
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
      const sdm::TrackerHitHdl& huclhit = tcd_sol.get_unclustered_hits()[j];
      const sdm::calibrated_tracker_hit& uclhit = huclhit.get();
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
