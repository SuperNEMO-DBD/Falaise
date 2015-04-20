// test_trackfit_helix_fit_mgr.cxx

// Standard library:
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <string>
#include <exception>
#include <time.h>
#include <list>
#include <unistd.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/placement.h>
#include <geomtools/helix_3d.h>
#include <geomtools/rectangle.h>

// This project:
#include <TrackFit/gg_hit.h>
#include <TrackFit/drawing.h>
#include <TrackFit/helix_fit_mgr.h>
#include <TrackFit/i_drift_time_calibration.h>

geomtools::helix_3d generate_helix_geiger_hits (mygsl::rng & random_,
                                                TrackFit::gg_hits_col & hits_,
                                                size_t nhits_,
                                                double rcell_,
                                                TrackFit::default_drift_time_calibration & dtc_,
                                                bool draw_ = false,
                                                bool no_z_ = false,
                                                bool bad_ = false);

// std::string gp_macro(const std::string & filename_);

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'TrackFit::helix_fit_mgr'!" << std::endl;

    bool debug          = false;
    bool devel          = false;
    bool draw           = false;
    bool use_drift_time = true;
    int  only_guess     = -1;
    size_t nhits        = 10;
    bool bad            = false;
    int  seed           = 0;
    bool no_z           = false;
    bool stop1          = false;
    bool do_fit         = true;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else if ((option == "-T")  || (option == "--no-drift-time")) {
          use_drift_time = false;
        } else if (option == "-z") {
          no_z = true;
        } else if (option == "-b") {
          bad = true;
        } else if (option == "-s") {
          seed = 0;
          //iarg++;
          //seed = atoi ( argv_[iarg] );
          //std::cerr << "DEBUG: seed = " << seed << std::endl;
        } else if (option == "-S1") {
          stop1 = true;
        } else if (option == "-f") {
          do_fit = false;
        } else if (option == "-G0") {
          only_guess = 0;
        } else if (option == "-G1") {
          only_guess = 1;
        } else if (option == "-G2") {
          only_guess = 2;
        } else if (option == "-G3") {
          only_guess = 3;
        } else if (option == "-G4") {
          only_guess = 4;
        } else if (option == "-G5") {
          only_guess = 5;
        } else if (option == "-G6") {
          only_guess = 6;
        } else if (option == "-G7") {
          only_guess = 7;
        } else if (option == "-h3") {
          nhits = 3;
        } else if (option == "-h10") {
          nhits = 10;
        } else if (option == "-h50") {
          nhits = 50;
        } else {
          std::clog << "warning: ignoring option '" << option << "' !" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "' !" << std::endl;
        }
      }
      iarg++;
    }

    std::clog << "warning: only_guess = '" << only_guess << "' !" << std::endl;

    // setup a pseudo-random number generator:
    std::string       rng_id   = "mt19937";
    unsigned long int rng_seed = seed;
    if (seed == 0) {
      /* get the current calendar time */
      long ltime = time (NULL);
      long stime = (unsigned) ltime / 2;
      rng_seed = stime;
    }

    std::cerr << "DEBUG: seed = " << rng_seed << std::endl;

    mygsl::rng random(rng_id, rng_seed);

    // define a drift_time calibration rule:
    TrackFit::default_drift_time_calibration dtc;

    if (devel)
      std::clog << "DEVEL: rmax = " << dtc.rmax / CLHEP::mm << " mm" << std::endl;

    // generate fake Geiger hits:
    TrackFit::gg_hits_col hits;
    geomtools::helix_3d trajectory
      = generate_helix_geiger_hits (random,
                                    hits,
                                    nhits,
                                    dtc.rmax,
                                    dtc,
                                    draw,
                                    no_z,
                                    bad);

    if (! use_drift_time) {
      for ( TrackFit::gg_hits_col::iterator it_hit = hits.begin ();
            it_hit != hits.end ();
            it_hit++) {
        it_hit->set_r (dtc.rmax);
        it_hit->set_sigma_r (0.);
      }
    }

    if (devel) {
      trajectory.tree_dump (std::clog, "Trajectory: ", "DEVEL: ");
    }
    if (stop1) return 0;

    // drawer:
    // if (draw)
    //   {
    //     TrackFit::drawing my_drawing;
    //     geomtools::placement pl (0, 0, 0, 0, 0, 0);
    //     my_drawing.draw (std::cout, pl, hits);
    //     std::cout << std::endl << std::endl;
    //   }

    // dummy:
    TrackFit::gg_hits_col hits_ref;
    geomtools::placement ref;
    hits_ref = hits;
    // {
    //   TrackFit::drawing my_drawing;
    //   geomtools::placement pl (0, 0, 0, 0, 0, 0);
    //   if (draw)
    //     {
    //       my_drawing.draw (std::cout, pl, hits_ref);
    //       std::cout << std::endl << std::endl;
    //     }
    // }
    std::cerr << "**************** ref done ********************" << std::endl;

    std::vector<TrackFit::helix_fit_params> guess(TrackFit::helix_fit_mgr::guess_utils::NUMBER_OF_GUESS);
    //TrackFit::helix_fit_params guess[TrackFit::helix_fit_mgr::guess_utils::NUMBER_OF_GUESS];
    int max_guess = TrackFit::helix_fit_mgr::guess_utils::NUMBER_OF_GUESS;

    // compute 8 different guesses:
    for (int iguess = 0; iguess < max_guess; iguess++) {
      if (only_guess >= 0) {
        if (iguess != only_guess) continue;
      }
      std::clog << "NOTICE: Compute guess #" << iguess << std::endl;
      datatools::properties config;
      TrackFit::helix_fit_mgr::guess_utils GU;
      GU.initialize (config);
      bool guess_ok = GU.compute_guess (hits_ref, iguess, guess[iguess], draw);
      if (! guess_ok) {
        std::clog << "NOTICE: Cannot compute guess #" << iguess << std::endl;
        guess[iguess].reset ();
        continue;
      }

      // if (draw)
      //   {
      //     std::cout << "# guess " << iguess << std::endl;
      //     std::cout << 0 << ' ' << 0 << ' ' << 0 << std::endl;
      //     std::cout << 0 << ' ' << 0 << ' ' << 0 << std::endl;
      //     //guess[iguess].draw (std::cout, 50. * CLHEP::cm);
      //   }
    }
    // if (draw)
    //   {
    //     std::cout << std::endl << std::endl;
    //   }

    //std::clog << "NOTICE: STOP" << std::endl;
    //exit (3);

    // do a fit for different starting guess:
    std::list<TrackFit::helix_fit_solution> solutions;
    if (do_fit)  {
      std::clog << "NOTICE: Perform the fit..." << std::endl;
      datatools::properties config;
      if (debug) {
        config.store_flag ("step_print_status");
      }
      if (draw) {
        config.store_flag ("step_draw");
      }
      if (! use_drift_time)  {
        config.store_flag ("ignore_drift_time");
      }

      for (int iguess = 0; iguess < max_guess; iguess++) {
        if (only_guess >=0) {
          if (iguess != only_guess) continue;
        }
        std::clog << "NOTICE: Starting fit for guess " << iguess << "..." << std::endl;
        TrackFit::helix_fit_mgr HFM;
        HFM.set_debug (debug);
        HFM.set_hits (hits_ref);
        HFM.set_calibration (dtc);
        HFM.set_t0 (0.0 * CLHEP::ns);
        double eps = 1.e-2;
        HFM.set_fit_eps (eps);
        HFM.set_guess (guess[iguess]);
        HFM.init (config);
        HFM.fit ();

        if (HFM.get_solution ().ok) {
          std::clog << "NOTICE: Solution in working frame has been found after "
                    << HFM.get_solution ().niter << " iterations:" << std::endl;
          //HFM.draw_solution (std::cout);
          std::clog << "NOTICE:   Chi     = "
                    << HFM.get_solution ().chi << std::endl;
          std::clog << "NOTICE:   N(dof)  = "
                    << HFM.get_solution ().ndof << std::endl;
          std::clog << "NOTICE:   Prob(P) = "
                    << HFM.get_solution ().probability_p () << std::endl;
          std::clog << "NOTICE:   Prob(Q) = "
                    << HFM.get_solution ().probability_q () << std::endl;
          for (int i = 0; i < (int) hits.size (); i++) {
            std::clog << "NOTICE:    Hit #" << i << " : ";
            double alpha_residual, beta_residual;
            HFM.get_residuals_per_hit (i, alpha_residual, beta_residual, true);
            std::clog << " Rai = " << alpha_residual;
            std::clog << " Rbi = " << beta_residual;
            std::clog << std::endl;
          }
          solutions.push_back (HFM.get_solution ());
        } else {
          std::clog << "NOTICE: No solution has been found !" << std::endl;
        }
        HFM.reset ();
      }
    }

    TrackFit::helix_fit_solution best_solution;
    double min_chi = -1.0;
    for (std::list<TrackFit::helix_fit_solution>::const_iterator i = solutions.begin ();
         i != solutions.end ();
         i++) {
      // i->print (std::clog);
      if ((min_chi < 0.0) || (i->chi < min_chi)) {
        min_chi = i->chi;
        best_solution = *i;
      }
    }
    std::clog << "NOTICE: Best solution: " << std::endl;
    best_solution.print (std::clog);
    if (best_solution.probability_q () < 0.2) {
      std::clog << "NOTICE: The probability is poor !" << std::endl;
    } else {
      std::clog << "NOTICE: The probability is good !" << std::endl;
    }

    //std::cout << std::endl << std::endl;

    geomtools::vector_3d center ( best_solution.x0,
                                  best_solution.y0,
                                  best_solution.z0 );
    double radius  = best_solution.r;
    double step    = best_solution.step;
    double angle_1 = best_solution.angle_1;
    double angle_2 = best_solution.angle_2;

    std::clog << "DEVEL: angle_1 = " << angle_1 / CLHEP::degree << " deg" << std::endl;
    std::clog << "DEVEL: angle_2 = " << angle_2 / CLHEP::degree << " deg" << std::endl;

    geomtools::helix_3d h3d;
    h3d.set_center ( center  );
    h3d.set_radius ( radius  );
    h3d.set_step   ( step    );
    h3d.set_angle1 ( angle_1 );
    h3d.set_angle2 ( angle_2 );

    trajectory.tree_dump ( std::cerr, "trajectory", "GUESS: " );
    h3d.tree_dump ( std::cerr, "h3d", "SOLUT: " );
    //geomtools::helix_3d::print_xyz (std::cout, h3d, 0.01, 0);
    datatools::temp_file ftmp;
    ftmp.set_remove_at_destroy (true);
    ftmp.create ("/tmp", "test_helix_fit_best_");
    TrackFit::helix_fit_mgr::draw_solution(ftmp.out(), best_solution);
    ftmp.out () << std::endl << std::endl;
    draw_hits (ftmp.out (), hits_ref);
    ftmp.out () << std::endl << std::endl;

    usleep (200.0);
    std::ostringstream title_oss;
    title_oss << "line_fit_mgr::at_fit_step_do : Best fit (XY-view)";

    if (draw) {
      Gnuplot g1 ("lines");
      g1.set_title (title_oss.str ());
      std::ostringstream cmd_oss;
      cmd_oss << "set size ratio -1; ";
      cmd_oss << "set grid; ";
      cmd_oss << "plot ";
      cmd_oss << " '" << ftmp.get_filename () << "' index 0 using 1:2 title \"Best solution\" with lines , ";
      cmd_oss << " '" << ftmp.get_filename () << "' index 1 using 1:2 title \"Hits\" with lines ";
      g1.cmd (cmd_oss.str ());
      g1.showonscreen (); // window output
      geomtools::gnuplot_drawer::wait_for_key ();
    }

    std::clog << "The end.\n";
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}


geomtools::helix_3d generate_helix_geiger_hits (mygsl::rng & random_,
                                                TrackFit::gg_hits_col & hits_,
                                                size_t nhits_,
                                                double rcell_,
                                                TrackFit::default_drift_time_calibration & dtc_,
                                                bool draw_,
                                                bool no_z_,
                                                bool bad_)
{
  bool devel = false;
  bool draw = draw_;
  using namespace geomtools;
  bool no_z = no_z_;
  hits_.clear ();
  datatools::temp_file ftmp;
  ftmp.set_remove_at_destroy (true);
  ftmp.create ("/tmp", "temp_helix_fit_step_");

  double point_sz = 2.5 * CLHEP::mm;

  double r = random_.flat (50 * CLHEP::cm, 200. * CLHEP::cm);
  double step = random_.flat (+50 * CLHEP::cm, +100. * CLHEP::cm);
  // double charge = -1.0;

  double x0, y0, z0;
  x0 = random_.flat (-25.0 * CLHEP::cm, 25.0 * CLHEP::cm);
  y0 = random_.flat (-25.0 * CLHEP::cm, 25.0 * CLHEP::cm);
  z0 = random_.flat (-50.0 * CLHEP::cm, 50.0 * CLHEP::cm);
  double angle1 = random_.flat (-150 * CLHEP::degree, +150 * CLHEP::degree);
  vector_3d center (x0, y0, z0);
  helix_3d h3d;
  h3d.set_center (center);
  h3d.set_radius (r);
  h3d.set_step (step);
  h3d.set_angle1 (angle1);
  h3d.set_angle2 (angle1 + 180 * CLHEP::degree); // updated at the end

  int ibad = -1;
  if (bad_) {
    ibad = 1 + random_.uniform_int (nhits_ - 2);
  }
  double dtheta = 3. * rcell_ / h3d.get_radius () * CLHEP::radian;
  double angle = angle1;
  for (int i = 0; i < (int) nhits_; i++) {
    vector_3d ti = h3d.get_point (angle / (2. * M_PI));
    double drift_radius = random_.flat (0.1 * CLHEP::mm, rcell_);
    double dri = drift_radius;
    if (i == ibad) {
      dri += random_.flat (5 * CLHEP::mm, 15 * CLHEP::mm);
    }
    if (random_.uniform () < 0.5) dri *= -1.0;
    double drift_time, sigma_drift_time;
    dtc_.radius_to_drift_time (drift_radius,
                               drift_time,
                               sigma_drift_time);
    drift_time = random_.gaussian (drift_time, 20.0 * CLHEP::ns);
    double sigma_drift_radius;
    if (drift_time < 20.0 * CLHEP::ns) drift_time = 20.0 * CLHEP::ns;
    dtc_.drift_time_to_radius (drift_time,
                               drift_radius,
                               sigma_drift_radius);

    double ri = r + dri;
    double xi = x0 + ri * cos (angle);
    double yi = y0 + ri * sin (angle);
    double z_sigma = 2.5 * CLHEP::cm;
    z_sigma = 2.5 * CLHEP::mm;
    double zi = random_.gaussian (ti.z (), z_sigma);
    if (no_z) {
      zi = 25.5 * CLHEP::cm;
    }

    if (devel)
     std:: clog << "DEVEL: generate_helix_geiger_hits: "
           << "xi = " << xi
           << " yi = " << yi
           << " zi = " << zi
           << " ri = " << drift_radius
           << " sigma_ri = " << sigma_drift_radius
           << " ti = " << drift_time
           << std::endl;

    angle += dtheta;

    TrackFit::gg_hit hit;
    hit.set_x (xi);
    hit.set_y (yi);
    hit.set_z (zi);
    hit.set_sigma_z (z_sigma);
    if (no_z) {
      hit.set_sigma_z (z_sigma / 20);
    }
    hit.set_r (drift_radius);
    hit.set_sigma_r (sigma_drift_radius);
    hit.set_t (drift_time);
    hit.set_rmax (rcell_);
    hits_.push_back (hit);
    hits_.back ().set_id (hits_.size () - 1);

    geomtools::vector_3d pos (xi, yi, zi);
    geomtools::rotation  rot;
    geomtools::gnuplot_draw::draw_circle (ftmp.out (), pos, rot, drift_radius);
    geomtools::rectangle rect(point_sz, point_sz);
    geomtools::gnuplot_draw::draw_rectangle (ftmp.out (), pos, rot, rect);
  }
  h3d.set_angle2 (angle);

  if ( devel ) {
    std::clog << "DEVEL: generate_helix_geiger_hits: "
         << "Helix radius= " << h3d.get_radius () / CLHEP::cm << " cm" << std::endl;
    std::clog << "DEVEL: generate_helix_geiger_hits: "
         << "Helix step= " << h3d.get_step () / CLHEP::cm << " cm" << std::endl;
  }

  geomtools::vector_3d pos;
  geomtools::rotation  rot;
  geomtools::basic_polyline_3d hpl;
  double delta_theta = 1. * CLHEP::degree;
  h3d.make_vertex_collection (hpl, delta_theta / CLHEP::radian);
  ftmp.out () << std::endl;
  geomtools::rectangle rect(point_sz, point_sz);
  geomtools::gnuplot_draw::draw_rectangle (ftmp.out (), h3d.get_center (), rot, rect);
  geomtools::gnuplot_draw::basic_draw_polyline (ftmp.out (), hpl);


  geomtools::vector_3d mid
    = h3d.get_point (0.5 *(h3d.get_angle1 () + h3d.get_angle2 ()) / (2 * M_PI));

  ftmp.out () << std::endl;
  geomtools::gnuplot_draw::basic_draw_point (ftmp.out (), h3d.get_center (), true);
  geomtools::gnuplot_draw::basic_draw_point (ftmp.out (), mid, true);

  usleep(200.0);
  if (draw) {
    Gnuplot g1("lines");
    std::ostringstream title_oss;
    title_oss << "test_helix_fit_mgr";
    g1.set_title (title_oss.str ());
    g1.set_grid ();
    g1.cmd ("set size ratio -1");
    g1.set_xlabel ("x").set_ylabel ("y");
    g1.plotfile_xy (ftmp.get_filename (), 1, 2, "Hits", "index 0", "with lines")
      .plotfile_xy (ftmp.get_filename (), 1, 2, "Helix", "index 1", "with lines")
      .plotfile_xy (ftmp.get_filename (), 1, 2, "Radius", "index 2", "with lines");
    g1.showonscreen (); // window output
    std::cerr << "**************** wait_for_key ********************" << std::endl;
    geomtools::gnuplot_drawer::wait_for_key ();
    usleep (200.0);
  }

  geomtools::helix_3d::print_xyz (std::cout, h3d, 0.01, 0);
  return h3d;
}

/*
std::string gp_macro(const std::string & filename_, int nloop_)
{
  std::ostringstream out;

  out << "cm=10. ; ";
  out << "sz=150. ; ";
  out << "set title 'TrackFit::helix_fit_mgr test program' ; ";
  out << "set xrange [*:*]; ";
  out << "set yrange [*:*]; ";
  out << "set zrange [*:*]; ";
  out << "set xlabel 'x (mm)'; ";
  out << "set ylabel 'y (mm)'; ";
  out << "set zlabel 'z (mm)'; ";

  out << "set title 'TrackFit::helix_fit_mgr: World coordinates system (X-Y-Z view)'; ";
  out << "set size ratio -1; ";
  out << "set grid; ";
  out << "plot '" << filename_ << "' index 0 using 1:2 title 'Cells' with lines; ";
  out << "pause -1 'Hit [Enter]...'; ";

  out << "sz=200.; ";
  out << "set xrange [-sz*cm:+sz*cm]; ";
  out << "set yrange [-sz*cm:+sz*cm]; ";
  out << "set zrange [-sz*cm:+sz*cm]; ";
  out << "set size ratio -1; ";
  out << "set grid; ";

  out << "print 'Fit loops...'; ";
  for (int iloop = 1; iloop <= nloop_; iloop++) {
    out << "print 'Step " << iloop << " (if any...)'; ";
    out << "plot ";
    out << " '" << filename_ << "' index 1 using 1:2 title 'Cells' with lines, ";
    out << " '" << filename_ << "' index 2 using 1:2 title 'Guess' with lines lt 3, ";
    out << " 'helix_fit_mgr.step_" << iloop << ".data' using 1:2 "
        << "title 'Iter " << iloop << "' with lines lt 4; ";
    out << "pause -1 'Hit [Enter]...'";
  }

  out << "plot ";
  out << " '" << filename_ << "' index 1 using 1:2 title 'Cells' with lines, ";
  out << " '" << filename_ << "' index 2 using 1:2 title 'Guess' with lines lt 3, ";
  out << " '" << filename_ << "' index 3 using 1:2 title 'Solution(s)' with lines lt 4;";
  out << "pause -1 'Hit [Enter]...';";

  out << "set title 'TrackFit::helix_fit_mgr: World coordinates system (X-Y view)';";
  out << "set size ratio -1;";
  out << "set grid;";

  out << "plot ";
  out << " '" << filename_ << "' index 0 using 1:2 title 'Generated' with lines lt 2, ";
  out << " '" << filename_ << "' index 1 using 1:2 title 'Cells' with lines lt 1, ";
  out << " '" << filename_ << "' index 3 using 1:2 title 'Solution(s)' with lines lt 4;";
  out << "pause -1 'Hit [Enter]...';";

  out << "set title 'TrackFit::helix_fit_mgr: World coordinates system (X-Z view)';";
  out << "set autoscale;";
  out << "set size ratio -1;";
  out << "set grid;";

  out << "plot ";
  out << " '" << filename_ << "' index 0 using 1:3 title 'Generated' with lines lt 2,"; \
  out << " '" << filename_ << "' index 1 using 1:3 title 'Cells' with lines lt 1, ";
  out << " '" << filename_ << "' index 3 using 1:3 title 'Solution(s)' with lines lt 4;";
  out << "pause -1 'Hit [Enter]...';";

  out << "set title 'TrackFit::helix_fit_mgr: World coordinates system (X-Y-Z view)';";
  out << "set autoscale;";
  out << "set size ratio -1;";
  out << "set grid;";

  out << "splot ";
  out << "      '" << filename_ << "' index 0 using 1:2:3 title 'Generated' with lines lt 2, ";
  out << "      '" << filename_ << "' index 1 using 1:2:3 title 'Cells' with lines lt 1, ";
  out << "      '" << filename_ << "' index 3 using 1:2:3 title 'Solution(s)' with lines lt 4;";
  out << "pause -1 'Hit [Enter]...';";
  return out.str();
}
*/
