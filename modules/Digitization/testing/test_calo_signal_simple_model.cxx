// test_calo_signal_simple_model.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
// - Bayeux/mygsl:
#include <mygsl/parameter_store.h>
// - Bayeux/mctools:
#include <mctools/signal/base_signal.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// This project:
#include <snemo/digitization/calo_signal_simple_shape.h>

void test_calo_signal_simple_model_1(bool draw_ = false);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::signal::base_signal'!" << std::endl;

    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-D" || arg == "--draw") draw = true;
      iarg++;
    }

    test_calo_signal_simple_model_1(draw);

    std::clog << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void test_calo_signal_simple_model_1(bool draw_)
{
  snemo::digitization::calo_signal_simple_shape csss1;
  csss1.set_polarity(mctools::signal::POLARITY_NEGATIVE);
  for (int i = 0; i < 100; i++)
    {
      double y = csss1(i);
      std::clog << "x = " << i << " y = " << y << std::endl;
    }



  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_calo_signal_simple_model+_");

  if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    Gnuplot g1;
    g1.cmd("set title 'Test mygsl::signal::calo_signal_simple_model' ");
    g1.cmd("set key out");
    g1.cmd("set grid");
    // // g1.cmd("set xrange [0:100]");
    // {
    //   std::ostringstream cmd1;
    //   cmd1 << "volt=" << CLHEP::volt;
    //   g1.cmd(cmd1.str());
    //   std::ostringstream cmd2;
    //   cmd2 << "nanosecond=" << CLHEP::nanosecond;
    //   g1.cmd(cmd2.str());
    // }
    // g1.cmd("set yrange [-1:+0.1]");
    // g1.set_xlabel("time (us)").set_ylabel("Signal (V)");

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  }

  return;
}
