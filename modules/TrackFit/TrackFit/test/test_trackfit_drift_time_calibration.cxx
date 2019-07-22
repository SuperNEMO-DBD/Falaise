// test_trackfit_drift_time_calibration.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <datatools/temporary_files.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/placement.h>

// This project:
#include <TrackFit/drawing.h>
#include <TrackFit/gg_hit.h>
#include <TrackFit/i_drift_time_calibration.h>

int main(int argc_, char** argv_) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'TrackFit::default_drift_time_calibration'!" << std::endl;

    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        { std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl; }
      }
      iarg++;
    }

    std::string rng_id = "mt19937";
    unsigned long int rng_seed = 12345;
    mygsl::rng random(rng_id, rng_seed);

    datatools::temp_file ftmp;
    ftmp.set_remove_at_destroy(true);
    ftmp.create("/tmp", "test_trackfit_drift_time_calibration_");

    TrackFit::default_drift_time_calibration dtc;
    for (double time = 0.0; time < 4000. * CLHEP::ns; time += 20. * CLHEP::ns) {
      double r, sig_r;
      dtc.drift_time_to_radius(time, r, sig_r);
      ftmp.out() << time << ' ' << r << ' ' << sig_r << std::endl;
    }
    ftmp.out() << std::endl << std::endl;

    TrackFit::gg_hits_col hits;
    for (int i = 0; i < 6; i++) {
      TrackFit::gg_hit hit;
      hit.set_x(5.0 * CLHEP::cm + i * 3.5 * CLHEP::cm);
      hit.set_y(1.25 * hit.get_x() + 4.5 * CLHEP::cm + random.flat(0.0, 3.0) * CLHEP::cm);
      double z_mu = 25.5 * CLHEP::cm + i * 3.2 * CLHEP::cm;
      double z_sigma = 1.2 * CLHEP::cm;
      hit.set_z(random.gaussian(z_mu, z_sigma));
      hit.set_sigma_z(z_sigma);
      hit.set_t(random.flat(20 * CLHEP::ns, 1400.0 * CLHEP::ns));
      double drift_radius, sigma_drift_radius;
      dtc.drift_time_to_radius(hit.get_t(), drift_radius, sigma_drift_radius);
      hit.set_r(drift_radius);
      hit.set_sigma_r(sigma_drift_radius);
      hits.push_back(hit);
    }

    TrackFit::drawing my_drawing;
    geomtools::placement pl(0, 0, 0, 0, 0, 0);
    my_drawing.draw(ftmp.out(), pl, hits);

    if (draw) {
      Gnuplot g1("lines");
      g1.set_title("TrackFit::drift_time_calibration");
      {
        std::ostringstream cmd_oss;
        cmd_oss << " mm=1; ns=1; cm=10.;"
                << " set title \"TrackFit::i_drift_time_calibration test program\"; "
                << " set xrange [0*ns:+4000*ns]; "
                << " set yrange [0*mm:+30*mm]; "
                << " set xlabel \"t (ns)\"; "
                << " set ylabel \"r (mm)\"; "
                << " set grid; ";
        cmd_oss << "plot ";
        cmd_oss << " '" << ftmp.get_filename() << "' index 0 using 1:2 notitle with lines , ";
        cmd_oss << " '" << ftmp.get_filename() << "' index 0 using 1:2:3 notitle with errorbars ";
        g1.cmd(cmd_oss.str());
        g1.showonscreen();  // window output
        geomtools::gnuplot_drawer::wait_for_key();
      }
      {
        std::ostringstream cmd_oss;
        cmd_oss << " cm=10.; sz=45.;"
                << " set xrange [0*cm:+sz*cm]; "
                << " set yrange [0*cm:+sz*cm]; "
                << " set zrange [0*cm:+sz*cm]; "
                << " set xlabel \"x (mm)\"; "
                << " set ylabel \"y (mm)\"; "
                << " set zlabel \"z (mm)\"; "
                << " set size ratio -1; "
                << " set grid; ";
        cmd_oss << "plot ";
        cmd_oss << " '" << ftmp.get_filename()
                << "' index 1 using 1:2 title \"Cells (X-Y view)\" with lines; pause 3; ";
        cmd_oss << "splot ";
        cmd_oss << " '" << ftmp.get_filename()
                << "' index 1 using 1:2:3 title \"Cells (3D view)\" with lines; ";
        g1.cmd(cmd_oss.str());
        g1.showonscreen();  // window output
        geomtools::gnuplot_drawer::wait_for_key();
      }
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
