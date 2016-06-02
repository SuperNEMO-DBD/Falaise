// test_snemo_geometry_mapped_magnetic_field.cxx
/*
 * Copyright 2006 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <falaise/snemo/geometry/mapped_magnetic_field.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/ioutils.h>
#include <datatools/clhep_units.h>
#include <datatools/library_loader.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/box.h>
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// This project:
#include <falaise/falaise.h>

int main (int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::geometry::mapped_magnetic_field' !" << std::endl;

    std::string map_filename;
    bool draw = false;
    bool trace = false;
    double yslice = datatools::invalid_real();
    bool z_inverted = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        std::string option = token;
        {
          if (option == "-D" || option == "--draw") {
            draw = true;
          } else if (option == "-T" || option == "--trace") {
            trace = true;
          } else if (option == "-Y" || option == "--y-slice") {
            yslice = 0.0;
          } else if (option == "-Z" || option == "--z-inverted") {
            z_inverted = true;
          } else {
            std::clog << datatools::io::warning << "ignoring option '"
                      << option << "'!" << std::endl;
          }
        }
      } else {
        std::string argument = token;
        {
          if (map_filename.empty()) {
            map_filename = argument;
          } else {
            std::clog << datatools::io::warning << "ignoring argument '"
                      << argument << "'!" << std::endl;
          }
        }
      }
      iarg++;
    }

    if (map_filename.empty()) {
      map_filename = "@falaise:config/snemo/demonstrator/geometry/4.0/plugins/magnetic_field/data/csv_map_0/MapSmoothPlusDetail.csv";
    }

    snemo::geometry::mapped_magnetic_field mmf;
    mmf.set_mapping_mode(snemo::geometry::mapped_magnetic_field::MM_IMPORT_CSV_MAP_0);
    mmf.set_map_filename(map_filename);
    mmf.set_zero_field_outside_map(true);
    mmf.set_z_inverted(z_inverted);
    if (trace) mmf.set_logging_priority(datatools::logger::PRIO_TRACE);
    mmf.initialize_simple();

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_snemo_geometry_mapped_magnetic_field_");

    double bscale = 0.005;

    {
      double time = 0.0 * CLHEP::nanosecond;
      geomtools::vector_3d position;
      geomtools::vector_3d B;
      double dx = 0.125 * CLHEP::m;
      double dy = 0.25 * CLHEP::m;
      double dz = 0.15 * CLHEP::m;
      double bmax=0.0;
      double bzmax=0.0;
      for (double x = -1.0 * CLHEP::m; x <= +1.0 * CLHEP::m; x += dx) {
        for (double y = -3.0 * CLHEP::m; y <= +3.0 * CLHEP::m; y += dy) {
          for (double z = -2.0 * CLHEP::m; z <= +2.0 * CLHEP::m; z += dz) {
            position.set(x, y, z);
            if (mmf.compute_magnetic_field(position, time, B)
                == snemo::geometry::mapped_magnetic_field::STATUS_SUCCESS) {
              bool save_it = true;
              if (datatools::is_valid(yslice)) {
                if ( std::abs(position.y() - yslice) > 0.25 * dy) {
                  save_it = false;
                }
              }
              if (save_it) {
                tmp_file.out() << position.x() / CLHEP::m << ' '
                               << position.y() / CLHEP::m << ' '
                               << position.z() / CLHEP::m << ' '
                               << bscale * B.x() / (CLHEP::gauss) << ' '
                               << bscale * B.y() / (CLHEP::gauss) << ' '
                               << bscale * B.z() / (CLHEP::gauss) << ' '
                               << std::endl;
              }
              if (B.mag() > bmax) {
                bmax = B.mag();
              }
              if (std::abs(B.z()) > bzmax) {
                bzmax = B.z();
              }
            }
          }
        }
      }
      std::clog << "B(max)  is : " << bmax / (CLHEP::gauss) << " G" << std::endl;
      std::clog << "Bz(max) is : " << bzmax / (CLHEP::gauss) << " G" << std::endl;
    }

    {
      // from Steve's Python test script:
      // At  1.6231 -1.3692 -0.337
      //     B=[-1662.2485000000004, 11250.897124999996, -9231.159109999995]
      // Somewhere in the middle of the tracking chamber:
      double x = -0.337  * CLHEP::m;
      double y =  1.6231 * CLHEP::m;
      double z = -1.3692 * CLHEP::m;
      // double x =  0.25  * CLHEP::m;
      // double y =  1.5 * CLHEP::m;
      // double z =  1.0 * CLHEP::m;
      geomtools::vector_3d position(x, y, z);
      geomtools::vector_3d B;
      mmf.compute_magnetic_field(position, 0.0, B);
      double b_unit = datatools::units::milli() * CLHEP::gauss;
      std::clog << "At  "
                << position.y() / CLHEP::m << ' '
                << position.z() / CLHEP::m << ' '
                << position.x() / CLHEP::m << ' '
                << "B=  ["
                << B.y() / b_unit << ", "
                << B.z() / b_unit << ", "
                << B.x() / b_unit << "]"
                << " mG"
                << std::endl;
      std::clog << "|B| = " << B.mag() / b_unit << " mG" << std::endl;
    }

    mmf.reset();

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test snemo::geometry::mapped_magnetic_field' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-0.5:+0.5]");
      g1.cmd("set yrange [-3:+3]");
      g1.cmd("set zrange [-2:+2]");
      g1.cmd("set xyplane at 0");
      g1.set_xlabel("x (m)").set_ylabel("y (m)").set_zlabel("z (m)");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' u 1:2:3:4:5:6 notitle 'Mapped magnetic field' with vectors";
        g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        g1.set_xlabel("x (m)").set_ylabel("z (m)");
        g1.cmd("set xrange [-0.75:+0.75]");
        g1.cmd("set yrange [-2:+2]");
        std::ostringstream plot_cmd;
        plot_cmd << "plot '" << tmp_file.get_filename() << "' u 1:3:4:6 notitle 'Mapped magnetic field' with vectors";
        g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
    }

    std::clog << "The end." << std::endl;

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "ERROR: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}
