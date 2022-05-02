// test_snemo_physics_model_sliced_activity_model.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/snemo/physics_model/constant_activity_model.h>
#include <falaise/snemo/physics_model/sliced_activity_model.h>
#include <falaise/snemo/time/time_utils.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::physics_model::sliced_activity_model'!" << std::endl;
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

  snpm::activity_model_dict_type amDict;
 
  {
    datatools::properties camConfig;
    camConfig.store_real_with_explicit_unit("activity", 0.05 * CLHEP::becquerel);
    camConfig.set_unit_symbol("activity", "Bq");

    auto cam = std::make_shared<snpm::constant_activity_model>();
    cam->initialize(camConfig, amDict);
    amDict.emplace("model1", cam);
   
    datatools::properties samConfig;
    samConfig.store("slice.start", "2022-05-27 02:13:42.678000");
    samConfig.store("slice.stop",  "2022-05-27 03:16:59.800999");
    samConfig.store("model", "model1");

    auto sam = std::make_shared<snpm::sliced_activity_model>();
    sam->initialize(samConfig, amDict);
    amDict.emplace("model2", sam);
  }
  
  auto cam = snpm::get_model(amDict, "model1");
  auto sam = snpm::get_model(amDict, "model2");
  
  snt::time_point start(snt::date(2022, 5, 27), snt::hours(1));
  snt::time_point stop(snt::date(2022, 5, 27), snt::hours(4));
  snt::time_point t = start;
  while (t < stop) {
    double a1 = cam->compute_activity(t);
    double a2 = sam->compute_activity(t);
    std::cout << snt::to_quantity(t - start) / CLHEP::second
              << ' ' << a1 / CLHEP::becquerel
              << ' ' << a2 / CLHEP::becquerel
              << std::endl;
    t += snt::minutes(1);
  }
 
  
  return;
}
