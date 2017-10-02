// test_trackfit_line_fit_mgr.cxx

// Standard library:
#include <time.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <list>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/line_3d.h>
#include <geomtools/placement.h>

// This project:
#include <TrackFit/drawing.h>
#include <TrackFit/gg_hit.h>
#include <TrackFit/i_drift_time_calibration.h>
#include <TrackFit/line_fit_mgr.h>

geomtools::line_3d generate_aligned_geiger_hits(mygsl::rng& random_, TrackFit::gg_hits_col& hits_,
                                                size_t nhits_, double rcell_,
                                                TrackFit::default_drift_time_calibration& dtc_,
                                                bool no_z_ = false, bool bad_ = false,
                                                bool draw_ = false,
                                                datatools::temp_file* ftmp_ = 0);

std::string gp_macro(const std::string& filename_);

int main(int argc_, char** argv_) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'TrackFit::line_fit_mgr'!" << std::endl;

    bool debug = false;
    bool draw = false;
    bool use_drift_time = true;
    int only_guess = -1;
    size_t nhits = 6;
    bool bad = false;
    bool seed = 314159;
    bool no_z = false;
    bool stop1 = false;
    bool do_fit = true;

    int iarg = 1;

    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else if ((option == "-T") || (option == "--no-drift-time")) {
          use_drift_time = false;
        } else if (option == "-z") {
          no_z = true;
        } else if (option == "-b") {
          bad = true;
        } else if (option == "-F") {
          do_fit = false;
        } else if (option == "-s") {
          seed = 0;
        } else if (option == "-S1") {
          stop1 = true;
        } else if (option == "-G0") {
          only_guess = 0;
        } else if (option == "-G1") {
          only_guess = 1;
        } else if (option == "-G2") {
          only_guess = 2;
        } else if (option == "-G3") {
          only_guess = 3;
        } else if (option == "-h3") {
          nhits = 3;
        } else if (option == "-h10") {
          nhits = 10;
        } else if (option == "-h50") {
          nhits = 50;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        { std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl; }
      }
      iarg++;
    }

    // Setup a pseudo-random number generator:
    std::string rng_id = "mt19937";
    unsigned long int rng_seed = seed;
    if (seed == 0) {
      /* Get the current calendar time */
      long ltime = time(NULL);
      long stime = (unsigned)ltime / 2;
      rng_seed = stime;
    }
    mygsl::rng random(rng_id, rng_seed);

    // Define a drift_time calibration rule:
    TrackFit::default_drift_time_calibration dtc;
    std::clog << "DEVEL: rmax = " << dtc.rmax / CLHEP::mm << " mm" << std::endl;

    // Generate fake Geiger hits:
    TrackFit::gg_hits_col hits;

    datatools::temp_file ftmp0;
    ftmp0.set_remove_at_destroy(true);
    ftmp0.create("/tmp", "test_line_fit_mgr_gen_");

    geomtools::line_3d trajectory =
        generate_aligned_geiger_hits(random, hits, nhits, dtc.rmax, dtc, no_z, bad, draw, &ftmp0);

    if (!use_drift_time) {
      for (TrackFit::gg_hits_col::iterator it_hit = hits.begin(); it_hit != hits.end(); it_hit++) {
        it_hit->set_r(dtc.rmax);
        it_hit->set_sigma_r(0.);
      }
    }

    if (stop1) return 0;

    datatools::temp_file ftmp;
    ftmp.set_remove_at_destroy(true);
    ftmp.create("/tmp", "test_trackfit_line_fit_mgr_");

    TrackFit::drawing my_drawing;
    geomtools::placement pl(0, 0, 0, 0, 0, 0);
    ftmp.out() << "# Hits in lab reference frame" << std::endl;
    my_drawing.draw(ftmp.out(), pl, hits);
    ftmp.out() << std::endl << std::endl;

    // Compute the hits within a working reference frame (w.r.f.) for the fit:
    // - in the working frame, the candidate track is garanted to be
    //   roughly aligned along the x'x axis.
    // - the fit is performed in this w.r.f. and possible solution of the fit
    //   have to be computed in the global reference frame (g.r.f.) at the end
    //   using the 'line_fit_mgr::convert_solution' method
    TrackFit::gg_hits_col hits_ref;
    geomtools::placement working_ref;
    TrackFit::line_fit_mgr::compute_best_frame(hits, hits_ref, working_ref);
    {
      TrackFit::drawing a_drawing;
      geomtools::placement a_pl(0, 0, 0, 0, 0, 0);
      ftmp.out() << "# Hits in working reference frame" << std::endl;
      a_drawing.draw(ftmp.out(), a_pl, hits_ref);
      ftmp.out() << std::endl << std::endl;
    }

    std::vector<TrackFit::line_fit_params> guess(
        TrackFit::line_fit_mgr::guess_utils::NUMBER_OF_GUESS);
    int max_guess = TrackFit::line_fit_mgr::guess_utils::NUMBER_OF_GUESS;

    // Compute 4 different guesses:
    ftmp.out() << "# 4 different guesses:" << std::endl;
    for (int iguess = 0; iguess < max_guess; iguess++) {
      if (only_guess >= 0) {
        if (iguess != only_guess) continue;
      }
      std::clog << "NOTICE: Compute guess #" << iguess << std::endl;
      datatools::properties config;
      TrackFit::line_fit_mgr::guess_utils GU;
      GU.initialize(config);
      bool guess_ok = GU.compute_guess(hits_ref, iguess, guess[iguess]);
      if (!guess_ok) {
        std::clog << "NOTICE: Cannot compute guess #" << iguess << std::endl;
        guess[iguess].reset();
        continue;
      }
      guess[iguess].draw(ftmp.out(), 50. * CLHEP::cm);
    }
    ftmp.out() << std::endl << std::endl;

    // Do a fit for different starting guess:
    std::list<TrackFit::line_fit_solution> solutions;
    if (do_fit) {
      std::clog << "NOTICE: Perform the fit..." << std::endl;
      datatools::properties config;
      if (debug) {
        config.store_flag("step_print_status");
      }
      if (draw) {
        config.store_flag("step_draw");
      }
      if (!use_drift_time) {
        config.store_flag("ignore_drift_time");
      }

      for (int iguess = 0; iguess < max_guess; iguess++) {
        if (only_guess >= 0) {
          if (iguess != only_guess) continue;
        }
        std::clog << "NOTICE: Starting fit for guess " << iguess << "..." << std::endl;
        TrackFit::line_fit_mgr LFM;
        LFM.set_hits(hits_ref);
        LFM.set_calibration(dtc);
        LFM.set_t0(0.0 * CLHEP::ns);
        LFM.set_debug(debug);
        double eps = 1.e-2;
        LFM.set_fit_eps(eps);
        LFM.set_guess(guess[iguess]);
        LFM.init(config);
        LFM.fit();

        if (LFM.get_solution().ok) {
          std::clog << "NOTICE: Solution in working frame has been found after "
                    << LFM.get_solution().niter << " iterations:" << std::endl;
          // LFM.draw_solution (std::cout);

          std::clog << "NOTICE:   Chi     = " << LFM.get_solution().chi << std::endl;
          std::clog << "NOTICE:   N(dof)  = " << LFM.get_solution().ndof << std::endl;
          std::clog << "NOTICE:   Prob(P) = " << LFM.get_solution().probability_p() << std::endl;
          std::clog << "NOTICE:   Prob(Q) = " << LFM.get_solution().probability_q() << std::endl;
          for (int i = 0; i < (int)hits.size(); i++) {
            std::clog << "NOTICE:    Hit #" << i << " : ";
            double alpha_residual, beta_residual;
            LFM.get_residuals_per_hit(i, alpha_residual, beta_residual, true);
            std::clog << " Rai = " << alpha_residual;
            std::clog << " Rbi = " << beta_residual;
            std::clog << std::endl;
          }
          if (LFM.get_solution().probability_q() > 0.1) {
            solutions.push_back(LFM.get_solution());
          }
        } else {
          std::clog << "NOTICE: No solution has been found !" << std::endl;
        }
        LFM.reset();
      }
    }
    // ftmp.out() << std::endl;

    // Find the best solution (with minimum chi-square):
    TrackFit::line_fit_solution best_solution;
    double min_chi = -1.0;
    for (std::list<TrackFit::line_fit_solution>::const_iterator i = solutions.begin();
         i != solutions.end(); i++) {
      i->tree_dump();
      if ((min_chi < 0.0) || (i->chi < min_chi)) {
        min_chi = i->chi;
        best_solution = *i;
      }
    }
    std::clog << "NOTICE: Best solution: " << std::endl;
    best_solution.tree_dump();
    if (best_solution.probability_q() < 0.2) {
      std::clog << "NOTICE: The probability is poor !" << std::endl;
    } else {
      std::clog << "NOTICE: The probability is good !" << std::endl;
    }

    {
      ftmp.out() << "# Best solution (in working reference frame): " << std::endl;
      TrackFit::line_fit_mgr LFM;
      LFM.set_hits(hits_ref);
      LFM.set_calibration(dtc);
      LFM.set_t0(0.0 * CLHEP::ns);
      LFM.draw_solution(ftmp.out(), best_solution);
      ftmp.out() << std::endl << std::endl;
    }

    // Compute the trajectory segment in the g.r.f (lab) frame:
    geomtools::line_3d best_track;
    TrackFit::line_fit_mgr::convert_solution(hits_ref, best_solution, working_ref, best_track);

    geomtools::vector_3d pos;
    geomtools::rotation rot;
    ftmp.out() << "# Best track segment: " << std::endl;
    geomtools::gnuplot_draw::draw_line(ftmp.out(), pos, rot, best_track);
    ftmp.out() << std::endl << std::endl;

    if (draw) {
      Gnuplot g1("lines");
      g1.set_title("TrackFit::line_fit_mgr");
      std::ostringstream cmd_oss;
      cmd_oss << gp_macro(ftmp.get_filename());
      std::cerr << "DEVEL: Gnuplot macro: " << std::endl;
      std::cerr << cmd_oss.str();
      std::cerr << std::endl;
      g1.cmd(cmd_oss.str());
      g1.showonscreen();  // window output
      geomtools::gnuplot_drawer::wait_for_key();
    }

    std::clog << "The end.\n";
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

geomtools::line_3d generate_aligned_geiger_hits(mygsl::rng& random_, TrackFit::gg_hits_col& hits_,
                                                size_t nhits_, double rcell_,
                                                TrackFit::default_drift_time_calibration& dtc_,
                                                bool no_z_, bool bad_, bool draw_,
                                                datatools::temp_file* ftmp_) {
  bool no_z = no_z_;
  hits_.clear();

  double theta0 = 30 * CLHEP::degree;
  double theta = (-1 + 2 * random_.uniform()) * theta0;
  // theta = theta0;
  // theta = 0;
  double xmin = 1.e40;
  double xmax = 0.0;
  geomtools::vector_3d start;
  geomtools::vector_3d stop;

  int ibad = -1;
  if (bad_) {
    ibad = 1 + random_.uniform_int(nhits_ - 2);
  }
  if (ftmp_) {
    ftmp_->out() << "# Generated hits: " << std::endl;
  }
  for (int i = 0; i < (int)nhits_; i++) {
    double x = random_.gaussian((i + 1) * 2. * rcell_ / cos(theta), 0.1 * rcell_);
    double drift_radius = random_.flat(0.1 * CLHEP::mm, rcell_);
    double y = drift_radius;
    if (i == ibad) {
      y += random_.flat(5 * CLHEP::mm, 15 * CLHEP::mm);
    }
    if (random_.uniform() < 0.5) y *= -1.0;
    double drift_time, sigma_drift_time;
    dtc_.radius_to_drift_time(drift_radius, drift_time, sigma_drift_time);
    drift_time = random_.gaussian(drift_time, 20.0 * CLHEP::ns);
    double sigma_drift_radius;
    if (drift_time < 20.0 * CLHEP::ns) drift_time = 20.0 * CLHEP::ns;
    dtc_.drift_time_to_radius(drift_time, drift_radius, sigma_drift_radius);

    double z_mu = 25.5 * CLHEP::cm + i * 3.2 * CLHEP::cm;
    double z_sigma = 2.5 * CLHEP::cm;
    double zi = random_.gaussian(z_mu, z_sigma);
    if (no_z) {
      zi = 25.5 * CLHEP::cm;
    }

    double xi = x * cos(theta) - y * sin(theta);
    double yi = x * sin(theta) + y * cos(theta);
    if (x < xmin) {
      xmin = x;
      start.set(x * cos(theta), x * sin(theta), zi);
    }
    if (x > xmax) {
      xmax = x;
      stop.set(x * cos(theta), x * sin(theta), zi);
    }
    std::clog << "DEVEL: generate_aligned_geiger_hits: "
              << "xi = " << xi << " yi = " << yi << " zi = " << zi << " ri = " << drift_radius
              << " sigma_ri = " << sigma_drift_radius << " ti = " << drift_time << std::endl;
    TrackFit::gg_hit hit;
    hit.set_id(i);
    hit.set_x(xi);
    hit.set_y(yi);
    hit.set_z(zi);
    hit.set_sigma_z(z_sigma);
    if (no_z) {
      hit.set_sigma_z(z_sigma / 20);
    }
    hit.set_r(drift_radius);
    hit.set_sigma_r(sigma_drift_radius);
    hit.set_t(drift_time);
    hit.set_rmax(rcell_);
    if (i == 0) hit.set_first(true);
    if (i == ((int)nhits_ - 1)) hit.set_last(true);
    hits_.push_back(hit);
    geomtools::vector_3d pos(xi, yi, zi);
    geomtools::rotation rot;
    if (ftmp_) {
      geomtools::gnuplot_draw::draw_circle(ftmp_->out(), pos, rot, drift_radius);
      geomtools::gnuplot_draw::draw_circle(ftmp_->out(), pos, rot, 0.1 * CLHEP::mm);
    }
  }

  if (draw_) {
    geomtools::vector_3d pos;
    geomtools::rotation rot;
    if (ftmp_) {
      geomtools::line_3d line(start, stop);
      geomtools::gnuplot_draw::draw_line(ftmp_->out(), pos, rot, line);
      Gnuplot g1("lines");
      std::ostringstream title_oss;
      title_oss << "test_line_fit_mgr";
      g1.set_title(title_oss.str());
      g1.set_grid();
      g1.cmd("set size ratio -1");
      g1.set_xlabel("x").set_ylabel("y");
      g1.plotfile_xy(ftmp_->get_filename(), 1, 2, "XY view");
      g1.showonscreen();  // window output
      std::cerr << "**************** wait_for_key ********************" << std::endl;
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
  }

  geomtools::line_3d l3d;
  l3d.set_first(start);
  l3d.set_last(stop);
  return l3d;
}

std::string gp_macro(const std::string& filename_) {
  std::ostringstream out;

  out << "cm=10. ; \n";
  out << "sz=50. ; \n";
  out << "set title 'TrackFit::line_fit_mgr test program'; \n";
  out << "set xrange [0*cm:+sz*cm]; \n";
  out << "set yrange [-0.5*sz*cm:+0.5*sz*cm]; \n";
  out << "set zrange [0*cm:+sz*cm]; \n";
  out << "set xlabel 'x (mm)'; \n";
  out << "set ylabel 'y (mm)'; \n";
  out << "set zlabel 'z (mm)'; \n";
  out << "set title 'TrackFit::line_fit_mgr: World coordinates system (XY view)'; \n";
  out << "set size ratio -1; \n";
  out << "set grid; \n";
  out << "plot ";
  out << "     '" << filename_ << "' index 0 using 1:2 title 'Cells' with lines ";
  out << "; \n";
  // out << "pause -1 'Hit [Enter]...'; \n";
  out << "pause 3; \n";

  out << "sz=20.; \n";
  out << "sz=25.; \n";
  out << "set xrange [-sz*cm:+sz*cm]; \n";
  out << "set yrange [-sz*cm:+sz*cm]; \n";
  out << "set zrange [-sz*cm:+sz*cm]; \n";
  out << "set title \"TrackFit::line_fit_mgr: Line-fit coordinates system (X'Y' view)\"; \n";
  out << "set size ratio -1; \n";
  out << "set grid; \n";
  out << "set view equal xyz; \n";
  out << "set xyplane at -200; \n";
  out << "splot ";
  out << "      '" << filename_ << "' index 1 using 1:2:3 title 'Cells' with lines ";
  out << "    , '" << filename_ << "' index 2 using 1:2:3 title 'Guess' with lines lt 3 ";
  out << "    , '" << filename_ << "' index 3 using 1:2:3 title 'Solution(s)' with lines lt 4 ";
  out << "; \n";
  // out << "pause -1 'Hit [Enter]...'; \n";
  out << "pause 3; \n";

  out << "sz=50.; \n";
  out << "set xrange [0*cm:+sz*cm]; \n";
  out << "set yrange [-sz*cm:+sz*cm]; \n";
  out << "set zrange [-sz*cm:+sz*cm]; \n";
  out << "set title 'TrackFit::line_fit_mgr: World coordinates system (XY view)'; \n";
  out << "set xrange [0*cm:+sz*cm]; \n";
  out << "set size ratio -1; \n";
  out << "set grid; \n";
  out << "plot ";
  out << "      '" << filename_ << "' index 0 using 1:2 title 'Cells' with lines ";
  out << "    , '" << filename_ << "' index 4 using 1:2 title 'Fitted segment' with lines ";
  out << "; \n";
  // out << "pause -1 'Hit [Enter]...'; \n";
  out << "pause 3; \n";

  out << "set title 'TrackFit::line_fit_mgr: World coordinates system (XY view)'; \n";
  out << "set xrange [0*cm:+sz*cm]; \n";
  out << "set grid; \n";
  out << "splot ";
  out << "      '" << filename_ << "' index 0 using 1:2:3 title 'Cells' with lines ";
  out << "    , '" << filename_ << "' index 4 using 1:2:3 title 'Fitted segment' with lines ";
  out << "; \n";
  // out << "pause -1 'Hit [Enter]...'; \n";
  out << "pause 3; \n";

  return out.str();
}
