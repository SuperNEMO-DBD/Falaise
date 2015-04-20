// test_tracker_clustering_solution.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>

// Third party:
// - Boost/datatools:
#include <datatools/clhep_units.h>
// - Boost/geomtools:
#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/geom_id.h>
#include <geomtools/rectangle.h>
#include <geomtools/line_3d.h>

// This project:
#include <falaise/snemo/datamodels/tracker_cluster.h>
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>

namespace sdm = snemo::datamodel;

void wait_for_key();  // Programm halts until keypress

geomtools::vector_3d locate_gg_cell(const geomtools::geom_id & gid)
{
  geomtools::vector_3d pos;
  // int module = gid.get(0);
  int side = gid.get(1);
  int layer = gid.get(2);
  int row = gid.get(3);

  double x = 10 + 22 + layer * 44;
  double y = -0.5 * 113 * 44 + row * 44;
  if(side == 0) {
    x *= -1;
  }
  double z = 0.0;
  pos.set( x, y, z);
  pos *= CLHEP::mm;
  return pos;
}

void draw_gg_hit(std::ostream & out_,
                 const sdm::calibrated_tracker_hit & gg_hit_)
{
  geomtools::vector_3d gg_pos = locate_gg_cell(gg_hit_.get_geom_id());
  double rdrift = gg_hit_.get_r();
  double rdrift_err = gg_hit_.get_sigma_r();
  double zdrift = gg_hit_.get_z();
  double zdrift_err = gg_hit_.get_sigma_z();

  geomtools::vector_3d hit_pos = gg_pos;
  geomtools::rotation_3d hit_rot;
  hit_pos.setZ(hit_pos.x() + zdrift);
  geomtools::gnuplot_draw::draw_circle(out_,
                                       hit_pos, hit_rot,
                                       rdrift-rdrift_err);
  geomtools::gnuplot_draw::draw_circle(out_,
                                       hit_pos, hit_rot,
                                       rdrift+rdrift_err);
  geomtools::vector_3d hit_segp;
  hit_segp.setZ(zdrift_err);
  geomtools::vector_3d hit_segm;
  hit_segm.setZ(-zdrift_err);
  geomtools::line_3d line(hit_segp, hit_segm);
  geomtools::gnuplot_draw::draw_line(out_, hit_pos, hit_rot, line);
  return;
}

void draw_gg_cluster_item(std::ostream & out_,
                          const sdm::calibrated_tracker_hit & gg_hit_,
                          bool enlarged_ = false)
{
  geomtools::vector_3d gg_pos = locate_gg_cell(gg_hit_.get_geom_id());
  geomtools::vector_3d hit_pos = gg_pos;
  double zdrift = gg_hit_.get_z();
  geomtools::rotation_3d hit_rot;
  hit_pos.setZ(hit_pos.x() + zdrift);
  double width = 42 * CLHEP::mm;
  if(enlarged_) width += 2 * CLHEP::mm;
  geomtools::rectangle rect(width, width);
  geomtools::gnuplot_draw::draw_rectangle(out_, hit_pos, hit_rot, rect);
  return;
}

void draw_gg_cluster(std::ostream & out_,
                     const sdm::tracker_cluster & gg_cluster_,
                     bool enlarged_ = false,
                     bool sep_ = true)
{
  for(int i = 0; i < (int) gg_cluster_.get_hits().size(); ++i) {
    draw_gg_cluster_item(out_, gg_cluster_.get_hits()[i].get(), enlarged_);
  }
  if(sep_) {
    out_ << std::endl << std::endl;
  }
  return;
}

void draw_gg_cluster_solution(std::ostream & out_,
                              const sdm::tracker_clustering_solution & gg_cluster_solution_,
                              const std::string & mode_,
                              bool sep_ = true)
{
  if(mode_ == "clusters") {
    for(int i = 0; i < (int) gg_cluster_solution_.get_clusters().size(); ++i) {
      draw_gg_cluster(out_,
                      gg_cluster_solution_.get_clusters()[i].get(),
                      true,
                      false);
    }
  }
  if(mode_ == "unclustered_hits") {
    for(int i = 0; i < (int) gg_cluster_solution_.get_unclustered_hits().size(); ++i) {
      draw_gg_cluster_item(out_,
                           gg_cluster_solution_.get_unclustered_hits()[i].get(),
                           true);
    }
  }
  if(sep_) {
    out_ << std::endl << std::endl;
  }
  return;
}

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'snemo::datamodel::tracker_clustering_solution' class." << std::endl;

    bool draw = false;
    int iarg = 1;
    while(iarg < argc_) {
      std::string token = argv_[iarg];
      if(token[0] == '-') {
        std::string option = token;
        if((option == "--no-draw")) {
          draw = false;
        } else if((option == "--draw")) {
          draw = true;
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

    srand48(314159);

    datatools::handle<std::ofstream> fvisu;
    std::string fvisuname = "test_tracker_clustering_solution_vis.data";
    if(draw) {
      fvisu.reset(new std::ofstream(fvisuname.c_str()));
    }

    // Populate a collection of handles on Geiger hits :
    sdm::calibrated_tracker_hit::collection_type hits;
    for(int i = 0; i < 18; ++i) {
      sdm::calibrated_tracker_hit::handle_type h(new sdm::calibrated_tracker_hit);
      sdm::calibrated_tracker_hit & gg_hit = h.grab();
      gg_hit.set_hit_id(i);
      geomtools::geom_id gid;
      gid.set_type(1204);
      if(i < 10) {
        gid.set_address(0, 0, i, 45 + i/2);
      } else if(i < 16) {
        gid.set_address(0, 1,(i - 10) / 2, 42 -(i - 10));
      } else if(i == 16) {
        gid.set_address(0, 1, 4, 45);
      } else if(i == 17) {
        gid.set_address(0, 1, 6, 48);
      }
      gg_hit.set_geom_id(gid);
      gg_hit.grab_auxiliaries().store_flag("fake");
      gg_hit.set_r( 22.*CLHEP::mm * drand48() );
      gg_hit.set_sigma_r( 1. * CLHEP::mm );
      gg_hit.set_z((50. + i) * CLHEP::cm +(-1. + 2 * drand48()) * CLHEP::cm );
      gg_hit.set_sigma_z( 0.5 * CLHEP::cm );
      hits.push_back(h);
      {
        std::ostringstream title;
        title << "Hit #" << i;
        gg_hit.tree_dump(std::clog, title.str());
      }
      if(draw) {
        draw_gg_hit(fvisu.grab(), gg_hit);
      }
    }
    if(draw) fvisu.grab() << std::endl << std::endl;

    // Create a handle on some tracker cluster :
    sdm::tracker_cluster::handle_type hTC0(new sdm::tracker_cluster);
    sdm::tracker_cluster & TC0 = hTC0.grab();
    TC0.set_cluster_id(0);
    TC0.make_prompt();
    TC0.grab_hits().push_back(hits[0]);
    TC0.grab_hits().push_back(hits[1]);
    TC0.grab_hits().push_back(hits[2]);
    TC0.grab_hits().push_back(hits[3]);
    TC0.grab_hits().push_back(hits[4]);
    TC0.grab_hits().push_back(hits[5]);
    TC0.grab_hits().push_back(hits[6]);
    TC0.grab_hits().push_back(hits[7]);
    TC0.grab_hits().push_back(hits[8]);
    TC0.grab_hits().push_back(hits[9]);
    TC0.grab_auxiliaries().store("display.color", "blue");
    {
      std::ostringstream title;
      title << "Tracker cluster #0";
      TC0.tree_dump(std::clog, title.str());
    }
    if(draw) {
      draw_gg_cluster(fvisu.grab(), TC0);
    }

    // Create another handle on some other tracker cluster :
    sdm::tracker_cluster::handle_type hTC1(new sdm::tracker_cluster);
    sdm::tracker_cluster & TC1 = hTC1.grab();
    TC1.set_cluster_id(1);
    TC1.make_prompt();
    TC1.grab_hits().push_back(hits[10]);
    TC1.grab_hits().push_back(hits[11]);
    TC1.grab_hits().push_back(hits[12]);
    TC1.grab_hits().push_back(hits[13]);
    TC1.grab_hits().push_back(hits[14]);
    TC1.grab_hits().push_back(hits[15]);
    TC1.grab_auxiliaries().store("display.color", "red");
    {
      std::ostringstream title;
      title << "Tracker cluster #1";
      TC1.tree_dump(std::clog, title.str());
    }
    if(draw) {
      draw_gg_cluster(fvisu.grab(), TC1);
    }

    sdm::tracker_clustering_solution::handle_type hTCS0(new sdm::tracker_clustering_solution);
    sdm::tracker_clustering_solution & TCS0 = hTCS0.grab();
    TCS0.set_solution_id(0);
    TCS0.grab_auxiliaries().store("weighting.chi2", 3.2546);
    TCS0.grab_auxiliaries().store("weighting.ndof", 5);
    TCS0.grab_unclustered_hits().push_back(hits[16]);
    TCS0.grab_unclustered_hits().push_back(hits[17]);
    TCS0.grab_clusters().push_back(hTC0);
    TCS0.grab_clusters().push_back(hTC1);
    {
      std::ostringstream title;
      title << "Tracker clustering solution #1";
      TCS0.tree_dump(std::clog, title.str());
    }
    if(draw) {
      draw_gg_cluster_solution(fvisu.grab(), TCS0, "clusters");
      draw_gg_cluster_solution(fvisu.grab(), TCS0, "unclustered_hits");
    }

    if(draw) {
      Gnuplot g1("lines");
      g1.set_title("test_tracker_clustering_solution");
      g1.set_xrange(-50*CLHEP::cm,+50*CLHEP::cm)
        .set_yrange(-100*CLHEP::cm,-10*CLHEP::cm)
        .set_zrange(-320*CLHEP::cm,+200*CLHEP::cm);
      std::ostringstream cmd_oss;
      cmd_oss << "set size ratio -1;";
      cmd_oss << "set grid;";
      cmd_oss << "plot ";
      cmd_oss << "'" << "test_tracker_clustering_solution_vis.data" << "' index 0 title 'hits' with lines ";
      cmd_oss << ", '" << "test_tracker_clustering_solution_vis.data" << "' index 1 title 'cluster #0' with lines";
      cmd_oss << ", '" << "test_tracker_clustering_solution_vis.data" << "' index 2 title 'cluster #1' with lines";
      cmd_oss << ", '" << "test_tracker_clustering_solution_vis.data" << "' index 3 title 'solution #0/clusters' with lines";
      cmd_oss << ", '" << "test_tracker_clustering_solution_vis.data" << "' index 4 title 'solution #0/unclustered' with lines";
      g1.cmd(cmd_oss.str());
      wait_for_key();
    }
  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}

void wait_for_key()
{
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
  std::cout << std::endl << "Press ENTER to continue..." << std::endl;
  std::cin.clear();
  std::cin.ignore(std::cin.rdbuf()->in_avail());
  std::cin.get();
#endif
  return;
}
