// test_snemo_physics_model_activity_model_factory.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/snemo/physics_model/activity_model_factory.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::physics_model::activity_model_factory'!" << std::endl;
    test1();

    std::clog << "The end." << std::endl;
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

void test1()
{
  namespace snt = snemo::time;
  namespace snpm = snemo::physics_model;

  snpm::activity_model_factory amFactory;
  datatools::properties amFactoryConfig;
  {
    std::string amFactoryConfigFilename = "${FALAISE_SNEMO_TESTING_DIR}/config/test-activity_model_factory.conf";
    datatools::fetch_path_with_env(amFactoryConfigFilename);
    amFactoryConfig.read_configuration(amFactoryConfigFilename);
  }
  amFactory.initialize(amFactoryConfig);
  std::clog << "Activity model factory:\n";
  amFactory.print_tree(std::clog);

  bool plotData = false;
  plotData = true;
  /* gnuplot:
     set grid
     set xlabel "Time (s)"
     set ylabel "Activity (Bq)"
     set xrange [0.0:]
     set yrange [0.0:0.005]
     plot 'test-activity_model_factory.data' u 1:3 w l , '' u 1:4 w l , '' u 1:5 w l, '' u 1:6 w l, '' u 1:7 w l lw 2
  */
  if (plotData) {
    std::ofstream fPlotData("test-activity_model_factory.data");
    snt::time_point start(snt::date(2021, 12, 25), snt::hours(0));
    snt::time_point stop(snt::date(2022, 2, 15), snt::hours(0));
    snt::time_point t = start;
    while (t < stop) {
      std::vector<double> acts;
      for (const auto & modEntry : amFactory.get_models()) {
        double act = modEntry.second->compute_activity(t);
        acts.push_back(act);
      }
      fPlotData << snt::to_quantity(t - start) / CLHEP::second;
      for (unsigned int i = 0; i < acts.size(); i++) {
        fPlotData  << ' ';
        fPlotData << acts[i] / CLHEP::becquerel;
      }
      fPlotData << std::endl;
      t += snt::minutes(15);
    }
  }
  
  return;
}
