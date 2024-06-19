// test_snemo_geometry_helix_intercept_1.cxx
/*
 * Copyright 2024 F. Mauger
 */

// Ourselves:
#include <falaise/snemo/geometry/helix_intercept.h>

// Bayeux:
#include <bayeux/geomtools/helix_3d.h>
#include <bayeux/geomtools/box.h>
#include <bayeux/datatools/temporary_files.h>
#include <bayeux/datatools/utils.h>
#include <bayeux/geomtools/geomtools_config.h>
#include <bayeux/geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <bayeux/geomtools/gnuplot_i.h>
#include <bayeux/geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// This project:
#include <falaise/falaise.h>

int main(int argc_, char** argv_)
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::geometry::helix_intercept' !" << std::endl;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        const std::string & option = token;
        {
          if (option == "-D" || option == "--draw") {
            draw = true;
          }

	}
      } else {
	const std::string & argument = token;
	{
	  std::clog << "[warning] " << "ignoring argument '" << argument << "'!"
		    << std::endl;
	}
      }
      iarg++;
    }	  

    geomtools::box block(0.8, 0.4, 0.1);
    block.lock();
    double x = -1.0;
    double y = 1.75;
    double z = 0.75;
    x += 0.0294;
    y = 1.52;
    // z = 0.905;
    geomtools::placement blockPlacement(x, y, z,
					0.0 * CLHEP::degree,
					90.0 * CLHEP::degree,
					0.0 * CLHEP::degree);


    double t1 = 0.0;
    double t2 = 0.3;
    geomtools::helix_3d helix;
    helix.set_radius(2.0);
    geomtools::vector_3d helixCenter(0., 0., 0.);
    helix.set_center(helixCenter);
    helix.set_step(1.5);
    helix.set_t1(t1);
    helix.set_t2(t2);
    helix.tree_dump(std::clog, "Helix:" );
    geomtools::helix_3d helix2;
    helix2.set_radius(2.0);
    helix2.set_center(helixCenter);
    helix2.set_step(1.5);
    helix2.set_t1(t1);
    helix2.set_t2(t2+0.2);
    
    geomtools::vector_3d first = helix.get_point(0.0);
    geomtools::vector_3d last = helix.get_point(0.5);
    std::clog << "first = " << first << "\n";
    std::clog << "last  = " << last << "\n";
  
    geomtools::placement helixPlacement(0., 0., 0.,
					0.0 * CLHEP::degree,
					0.0 * CLHEP::degree,
					0.0);

    double step = 0.5;
    double precision = 0.02;
    datatools::logger::priority verbosity = datatools::logger::PRIO_DEBUG;
    snemo::geometry::helix_intercept helixInter(helix, block, blockPlacement,
						step,
						precision,
						verbosity,
						1);
    snemo::geometry::vertex_info::from_bit_type fromBit = snemo::geometry::vertex_info::FROM_LAST_BIT;
    snemo::geometry::helix_intercept::extrapolation_info extrapolInfo;
    if (helixInter.find_intercept(extrapolInfo, fromBit)) {
      std::clog << "Found intercept.\n";
    } else {
      std::clog << "No intercept.\n";
    }
    
    datatools::temp_file tmpFile;
    tmpFile.set_remove_at_destroy(true);
    tmpFile.create("/tmp", "test_snemo_geometry_helix_intercept_1_");
    {
      // Draw helix (index 0):
      tmpFile.out() << "# helix 1: " << std::endl;
      geomtools::gnuplot_draw::draw_helix(tmpFile.out(),
                                          helixPlacement,
                                          helix,
                                          geomtools::helix_3d::WR_BASE_VERY_HIGH_ANGLE_SAMPLING | geomtools::helix_3d::WR_HELIX_NO_ANGULAR_SAMPLE_BOOST);
      tmpFile.out() << std::endl;
      // Draw helix2 (index 1):
      tmpFile.out() << "# helix 2: " << std::endl;
      geomtools::gnuplot_draw::draw_helix(tmpFile.out(),
                                          helixPlacement,
                                          helix2,
                                          geomtools::helix_3d::WR_BASE_VERY_HIGH_ANGLE_SAMPLING | geomtools::helix_3d::WR_HELIX_NO_ANGULAR_SAMPLE_BOOST);
      tmpFile.out() << std::endl;

      // Draw block (index 2):
      tmpFile.out() << "# box (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_box(tmpFile.out(),
                                        blockPlacement,
                                        block);
      tmpFile.out() << std::endl << std::endl;
     
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test snemo::geometry::helix\\_intercept' ");
      g1.cmd("set grid");
      g1.cmd("set key out");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-3:+3]");
      g1.cmd("set yrange [-3:+3]");
      g1.cmd("set zrange [-3:+3]");
      g1.cmd("set xyplane at -3");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmpFile.get_filename()
                 << "' index 0 title 'Helix' with lines ";
        plot_cmd << ", '' index 1 title 'Helix (more)' with lines ";
	plot_cmd << ", '' index 2 title 'Block' with lines ";
        plot_cmd << ", 'helix_intercept.data' index 0 title 'Extrap.' with linespoints pt 6 ps 0.5";
        plot_cmd << ", 'helix_intercept.data' index 1 title 'Impact.' with points pt 6 ps 0.85 lc rgb 'red'";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
	g1.set_xlabel("x").set_ylabel("y");
        std::ostringstream plot_cmd;
        plot_cmd << "plot '" << tmpFile.get_filename()
                 << "' index 0 title 'Helix' with lines ";
        plot_cmd << ", '' index 1 title 'Helix (more)' with lines ";
        plot_cmd << ", '' index 2 title 'Block' with lines ";
        plot_cmd << ", 'helix_intercept.data' index 0 title 'Extrap.' with linespoints pt 6 ps 0.5";
        plot_cmd << ", 'helix_intercept.data' index 1 title 'Impact.' with linespoints pt 6 ps 0.85 lc rgb 'red' ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
	g1.set_xlabel("y").set_ylabel("z");
        std::ostringstream plot_cmd;
        plot_cmd << "plot '" << tmpFile.get_filename()
                 << "' index 0 u 2:3 title 'Helix' with lines ";
        plot_cmd << ", '' index 1 u 2:3 title 'Helix (more)' with lines ";
        plot_cmd << ", '' index 2 u 2:3 title 'Block' with lines ";
        plot_cmd << ", 'helix_intercept.data' index 0 u 2:3 title 'Extrap.' with linespoints pt 6 ps 0.5";
        plot_cmd << ", 'helix_intercept.data' index 1 u 2:3 title 'Impact.' with linespoints pt 6 ps 0.85 lc rgb 'red'";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
	g1.set_xlabel("x").set_ylabel("z");
        std::ostringstream plot_cmd;
        plot_cmd << "plot '" << tmpFile.get_filename()
                 << "' index 0 u 1:3 title 'Helix' with lines ";
        plot_cmd << ", '' index 1 u 1:3 title 'Helix (more)' with lines ";
        plot_cmd << ", '' index 2 u 1:3 title 'Block' with lines ";
        plot_cmd << ", 'helix_intercept.data' index 0 u 1:3 title 'Extrap.' with linespoints pt 6 ps 0.5";
        plot_cmd << ", 'helix_intercept.data' index 1 u 1:3 title 'Impact.' with linespoints pt 6 ps 0.85 lc rgb 'red'";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

  } catch (std::exception& x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "ERROR: "
	      << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}
