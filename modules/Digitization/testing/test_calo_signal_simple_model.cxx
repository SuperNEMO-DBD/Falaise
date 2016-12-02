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
#include <snemo/digitization/calo_analog_signal.h>

void test_calo_signal_simple_model_1(bool draw_ = false);
void test_calo_signal_simple_model_2(bool draw_ = false);

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

    test_calo_signal_simple_model_2(draw);

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
  //  std::string input_datafile = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/calo_signal_shape_1300kev_mean_8000_signals.data";
  std::string input_datafile = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/calo_signal_reference_shape_reduced_1000kev.data";
  datatools::fetch_path_with_env(input_datafile);
  csss1.load_from_file(input_datafile);
  double dx = 0.01;
  std::clog << "Xmin = " << csss1.x_min() << " Xmax = " << csss1.x_max() << std::endl;
  std::clog << "Interpolation = " << csss1.interpolator_name() << std::endl;
  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_calo_signal_simple_model+_");  
  
  tmp_file.out() << "#" << csss1.interpolator_name() << std::endl;
  for (double x = csss1.x_min (); x <= csss1.x_max() + 0.001 * dx; x += dx) {
    if (csss1.is_valid(x)) {
      tmp_file.out() << x << ' ' << csss1(x) << std::endl;
    }
  }
  std::cout << std::endl;
  csss1.print_points(std::clog, "Before scale");
  csss1.unlock_table();
  csss1.scale(400);
  csss1.relock_table();
  csss1.print_points(std::clog, "After scale");

  double test_time = 50.0001;
  std::clog << "csss1(" << test_time << ") = " << csss1(test_time) << std::endl;
  datatools::temp_file tmp_file_2;
  tmp_file_2.set_remove_at_destroy(true);
  tmp_file_2.create("/tmp", "test_calo_signal_simple_model+_");  
  
  tmp_file_2.out() << "#" << csss1.interpolator_name() << std::endl;
  for (double x = csss1.x_min (); x <= csss1.x_max() + 0.001 * dx; x += dx) {
    if (csss1.is_valid(x)) {
      tmp_file_2.out() << x << ' ' << csss1(x) << std::endl;
    }
  }
  
  if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    Gnuplot g1;
    g1.cmd("set title 'Test snemo::digitization::calo_signal_simple_shape model' ");
    g1.cmd("set key out");
    g1.cmd("set grid");
    {
      std::ostringstream plot_cmd;
      plot_cmd << "plot '" << tmp_file_2.get_filename() << "' "
	       << " index 0"
	       << " title 'Calo Signal simple shape 1' with lines lw 4";
      g1.cmd(plot_cmd.str());
    }

    g1.cmd("set xrange [0:100]");
    {
      std::ostringstream cmd1;
      cmd1 << "volt=" << CLHEP::volt;
      g1.cmd(cmd1.str());
      std::ostringstream cmd2;
      cmd2 << "nanosecond=" << CLHEP::nanosecond;
      g1.cmd(cmd2.str());
    }
    g1.cmd("set yrange [-1:+0.1]");
    g1.set_xlabel("time (us)").set_ylabel("Signal (V)");
    g1.showonscreen(); // window output
    geomtools::gnuplot_drawer::wait_for_key();
    usleep(200);

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  }

  return;
}

void test_calo_signal_simple_model_2(bool draw_)
{  
  snemo::digitization::calo_signal_simple_shape csss1;
  //  std::string input_datafile = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/calo_signal_shape_1300kev_mean_8000_signals.data";
  std::string input_datafile = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/calo_signal_reference_shape_reduced_1000kev.data";
  datatools::fetch_path_with_env(input_datafile);
  csss1.load_from_file(input_datafile);
  
  double energy = 750; // keV
  double time_shift = 150; // ns
  
  snemo::digitization::calo_analog_signal cas1;
  cas1.set_energy_max(energy);
  cas1.set_signal_time(time_shift);
  cas1.initialize(csss1);

  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_calo_signal_simple_model+_");  

  double dx = 0.01;
  for (double x = cas1.get_analog_signal_shape().x_min (); x <= cas1.get_analog_signal_shape().x_max() + 0.001 * dx; x += dx) {
    if (cas1.get_analog_signal_shape().is_valid(x)) {
      tmp_file.out() << x << ' ' << cas1.get_analog_signal_shape()(x) << std::endl;
    }
  }
  
  if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    Gnuplot g1;
    g1.cmd("set title 'Test mygsl::signal::calo_signal_simple_model' ");
    g1.cmd("set key out");
    g1.cmd("set grid");
    {
      std::ostringstream plot_cmd;
      plot_cmd << "plot '" << tmp_file.get_filename() << "' "
	       << " index 0"
	       << " title 'Calo Signal simple shape 1' with lines lw 4";
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

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
