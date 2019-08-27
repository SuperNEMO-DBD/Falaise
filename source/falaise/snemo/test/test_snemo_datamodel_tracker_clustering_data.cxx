// test_tracker_clustering_data.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// Third party:
// - Boost/datatools:
#include <datatools/clhep_units.h>
// - Boost/geomtools:
#include <geomtools/geom_id.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/line_3d.h>
#include <geomtools/rectangle.h>
#include <geomtools/utils.h>

// This project:
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

namespace sdm = snemo::datamodel;

void wait_for_key();  // Program halts until keypress

/// Locator emulator :
geomtools::vector_3d locate_gg_cell(const geomtools::geom_id& gid) {
  geomtools::vector_3d pos;
  // int module = gid.get(0);
  int side = gid.get(1);
  int layer = gid.get(2);
  int row = gid.get(3);

  double x = 10 + 22 + layer * 44;
  double y = -0.5 * 113 * 44 + row * 44;
  if (side == 0) {
    x *= -1;
  }
  double z = 0.0;
  pos.set(x, y, z);
  pos *= CLHEP::mm;
  return pos;
}

void draw_gg_hit(std::ostream& out_, const sdm::calibrated_tracker_hit& gg_hit_) {
  geomtools::vector_3d gg_pos = locate_gg_cell(gg_hit_.get_geom_id());
  double rdrift = gg_hit_.get_r();
  double rdrift_err = gg_hit_.get_sigma_r();
  double zdrift = gg_hit_.get_z();
  double zdrift_err = gg_hit_.get_sigma_z();

  geomtools::vector_3d hit_pos = gg_pos;
  geomtools::rotation_3d hit_rot;
  hit_pos.setZ(hit_pos.x() + zdrift);
  geomtools::gnuplot_draw::draw_circle(out_, hit_pos, hit_rot, rdrift - rdrift_err);
  geomtools::gnuplot_draw::draw_circle(out_, hit_pos, hit_rot, rdrift + rdrift_err);
  geomtools::vector_3d hit_segp;
  hit_segp.setZ(zdrift_err);
  geomtools::vector_3d hit_segm;
  hit_segm.setZ(-zdrift_err);
  geomtools::line_3d line(hit_segp, hit_segm);
  geomtools::gnuplot_draw::draw_line(out_, hit_pos, hit_rot, line);
}

void draw_gg_cluster_item(std::ostream& out_, const sdm::calibrated_tracker_hit& gg_hit_,
                          bool enlarged_ = false) {
  geomtools::vector_3d gg_pos = locate_gg_cell(gg_hit_.get_geom_id());
  geomtools::vector_3d hit_pos = gg_pos;
  double zdrift = gg_hit_.get_z();
  geomtools::rotation_3d hit_rot;
  hit_pos.setZ(hit_pos.x() + zdrift);
  double width = 42 * CLHEP::mm;
  if (enlarged_) {
    width += 2 * CLHEP::mm;
  }
  geomtools::rectangle rect(width, width);
  geomtools::gnuplot_draw::draw_rectangle(out_, hit_pos, hit_rot, rect);
}

void draw_gg_cluster(std::ostream& out_, const sdm::tracker_cluster& gg_cluster_,
                     bool enlarged_ = false, bool sep_ = true) {
  for (const auto& i : gg_cluster_.get_hits()) {
    draw_gg_cluster_item(out_, i.get(), enlarged_);
  }
  if (sep_) {
    out_ << std::endl << std::endl;
  }
}

void draw_gg_clustering_solution(std::ostream& out_,
                                 const sdm::tracker_clustering_solution& gg_cluster_solution_,
                                 const std::string& mode_, bool sep_ = true) {
  if (mode_ == "clusters") {
    for (const auto& i : gg_cluster_solution_.get_clusters()) {
      draw_gg_cluster(out_, i.get(), true, false);
    }
  }
  if (mode_ == "unclustered_hits") {
    for (const auto& i : gg_cluster_solution_.get_unclustered_hits()) {
      draw_gg_cluster_item(out_, i.get(), true);
    }
  }
  if (sep_) {
    out_ << std::endl << std::endl;
  }
}

int main(int argc_, char** argv_) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'snemo::datamodel::tracker_clustering_data' class."
              << std::endl;

    bool draw = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        const std::string& option = token;
        if ((option == "--no-draw")) {
          draw = false;
        } else if ((option == "--draw")) {
          draw = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        const std::string& argument = token;
        { std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl; }
      }
      iarg++;
    }

    srand48(314159);

    datatools::handle<std::ofstream> fvisu;
    std::string fvisuname = "test_tracker_clustering_data_vis.data";
    if (draw) {
      fvisu.reset(new std::ofstream(fvisuname.c_str()));
    }

    // Event record :
    sdm::event_record ER;

    // Event header bank :
    auto& EH = ER.add<sdm::event_header>(sdm::data_info::default_event_header_label());
    EH.set_id(datatools::event_id(666, 345));
    EH.set_timestamp(sdm::timestamp(1268644034, 1204));
    EH.set_generation(sdm::event_header::GENERATION_SIMULATED);
    EH.get_properties().store_flag("fake");
    EH.get_properties().store("date", 20120306);
    EH.tree_dump(std::clog, "Event header('EH'): ");

    // Calibrated data bank :
    auto& CD = ER.add<sdm::calibrated_data>(sdm::data_info::default_calibrated_data_label());
    // Populate a collection of handles on Geiger hits :
    for (int i = 0; i < 18; ++i) {
      sdm::calibrated_tracker_hit::handle_type h(new sdm::calibrated_tracker_hit);
      sdm::calibrated_tracker_hit& gg_hit = h.grab();
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
      {
        std::ostringstream title;
        title << "Hit #" << i;
        gg_hit.tree_dump(std::clog, title.str());
      }
      if (draw) {
        draw_gg_hit(fvisu.grab(), gg_hit);
      }
      CD.calibrated_tracker_hits().push_back(h);
    }
    std::clog << std::endl;
    if (draw) {
      fvisu.grab() << std::endl << std::endl;
    }
    CD.tree_dump(std::clog, "Calibrated data('CD'): ");
    std::clog << std::endl;

    // Tracker clustering data bank :
    auto& TCD = ER.add<sdm::tracker_clustering_data>(
        sdm::data_info::default_tracker_clustering_data_label());

    // Get a reference to the collection of calibrated Geiger hits from the 'CD' bank :
    sdm::calibrated_data::tracker_hit_collection_type& gg_hits = CD.calibrated_tracker_hits();

    // Create a handle on some tracker cluster :
    sdm::tracker_cluster::handle_type hTC0(new sdm::tracker_cluster);
    sdm::tracker_cluster& TC0 = hTC0.grab();
    TC0.set_cluster_id(0);
    TC0.make_prompt();
    TC0.get_hits().push_back(gg_hits[0]);
    TC0.get_hits().push_back(gg_hits[1]);
    TC0.get_hits().push_back(gg_hits[2]);
    TC0.get_hits().push_back(gg_hits[3]);
    TC0.get_hits().push_back(gg_hits[4]);
    TC0.get_hits().push_back(gg_hits[5]);
    TC0.get_hits().push_back(gg_hits[6]);
    TC0.get_hits().push_back(gg_hits[7]);
    TC0.get_hits().push_back(gg_hits[8]);
    TC0.get_hits().push_back(gg_hits[9]);
    TC0.grab_auxiliaries().store("display.color", "blue");
    {
      std::ostringstream title;
      title << "Tracker cluster #0";
      TC0.tree_dump(std::clog, title.str());
      std::clog << std::endl;
    }

    // Create another handle on some other tracker cluster :
    sdm::tracker_cluster::handle_type hTC1(new sdm::tracker_cluster);
    sdm::tracker_cluster& TC1 = hTC1.grab();
    TC1.set_cluster_id(1);
    TC1.make_prompt();
    TC1.get_hits().push_back(gg_hits[10]);
    TC1.get_hits().push_back(gg_hits[11]);
    TC1.get_hits().push_back(gg_hits[12]);
    TC1.get_hits().push_back(gg_hits[13]);
    TC1.get_hits().push_back(gg_hits[14]);
    TC1.get_hits().push_back(gg_hits[15]);
    TC1.grab_auxiliaries().store("display.color", "red");
    {
      std::ostringstream title;
      title << "Tracker cluster #1";
      TC1.tree_dump(std::clog, title.str());
      std::clog << std::endl;
    }
    if (draw) {
      draw_gg_cluster(fvisu.grab(), TC0);
      draw_gg_cluster(fvisu.grab(), TC1);
    }

    sdm::tracker_clustering_solution::handle_type hTCS0(new sdm::tracker_clustering_solution);
    sdm::tracker_clustering_solution& TCS0 = hTCS0.grab();
    TCS0.set_solution_id(0);
    TCS0.get_auxiliaries().store_real("weighting.chi2", 3.2546);
    TCS0.get_auxiliaries().store_integer("weighting.ndof", 5);
    TCS0.get_clusters().push_back(hTC0);
    TCS0.get_clusters().push_back(hTC1);
    TCS0.get_unclustered_hits().push_back(gg_hits[16]);
    TCS0.get_unclustered_hits().push_back(gg_hits[17]);
    {
      std::ostringstream title;
      title << "Tracker clustering solution #0";
      TCS0.tree_dump(std::clog, title.str());
      std::clog << std::endl;
    }
    if (draw) {
      draw_gg_clustering_solution(fvisu.grab(), TCS0, "clusters");
      draw_gg_clustering_solution(fvisu.grab(), TCS0, "unclustered_hits");
    }

    TCD.add_solution(hTCS0, true);
    TCD.tree_dump(std::clog, "Tracker clustering data('TCD') : ");
    std::clog << std::endl;

    ER.tree_dump(std::clog, "Event record : ");

    if (draw) {
      Gnuplot g1("lines");
      g1.set_title("test_tracker_clustering_data");
      g1.set_xrange(-50 * CLHEP::cm, +50 * CLHEP::cm)
          .set_yrange(-100 * CLHEP::cm, -10 * CLHEP::cm)
          .set_zrange(-320 * CLHEP::cm, +200 * CLHEP::cm);
      std::ostringstream cmd_oss;
      cmd_oss << "set size ratio -1;";
      cmd_oss << "set grid;";
      cmd_oss << "plot ";
      cmd_oss << "  '" << fvisuname << "' index 0 title 'hits' with lines ";
      cmd_oss << ", '" << fvisuname << "' index 1 title 'cluster #0' with lines";
      cmd_oss << ", '" << fvisuname << "' index 2 title 'cluster #1' with lines";
      cmd_oss << ", '" << fvisuname << "' index 3 title 'solution #0/clusters' with lines";
      cmd_oss << ", '" << fvisuname << "' index 4 title 'solution #0/unclustered' with lines";
      g1.cmd(cmd_oss.str());
      wait_for_key();
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

void wait_for_key() {
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
  std::cout << std::endl << "Press ENTER to continue..." << std::endl;
  std::cin.clear();
  std::cin.ignore(std::cin.rdbuf()->in_avail());
  std::cin.get();
#endif
}
