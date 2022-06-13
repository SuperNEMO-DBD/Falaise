// test_snemo_physics_model_per_period_activity_model.cxx

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
#include <falaise/snemo/physics_model/scaled_activity_model.h>
#include <falaise/snemo/physics_model/sliced_activity_model.h>
#include <falaise/snemo/physics_model/per_period_activity_model.h>
#include <falaise/snemo/time/time_utils.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::physics_model::per_period_activity_model'!" << std::endl;
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

  snpm::per_period_activity_model ppam;
  ppam.add_slice_with_const_activity(snt::time_period_from_string("[2022-05-27 02:00:00/2022-05-27 03:30:00)"), 1.2 * CLHEP::becquerel);
  std::clog << "\nThis activity model:\n";
  ppam.print_tree(std::clog);

  ppam.add_slice_with_const_activity(snt::time_period_from_string("[2022-05-27 03:45:00/2022-05-27 04:25:00)"), 0.9 * CLHEP::becquerel);
  ppam.add_slice_with_const_activity(snt::time_period_from_string("[2022-05-27 04:25:00/2022-05-27 07:15:00)"), 0.95 * CLHEP::becquerel);
  ppam.add_slice_with_const_activity(snt::time_period_from_string("[2022-05-27 07:15:00/2022-05-27 10:45:00)"), 1.1 * CLHEP::becquerel);
  ppam.add_slice_with_const_activity(snt::time_period_from_string("[2022-05-27 10:45:00/2022-05-27 13:30:00)"), 1.5 * CLHEP::becquerel);
  ppam.add_slice_with_const_activity(snt::time_period_from_string("[2022-05-27 13:50:00/2022-05-27 16:15:00)"), 1.25 * CLHEP::becquerel);
  ppam.add_slice_with_const_activity(snt::time_period_from_string("[2022-05-27 16:15:00/2022-05-27 18:30:00)"), 1.0 * CLHEP::becquerel);
  ppam.initialize_simple();
  std::clog << "\nThis activity model:\n";
  ppam.print_tree(std::clog);
  
  snt::time_point start(snt::date(2022, 5, 27), snt::hours(1));
  snt::time_point stop(snt::date(2022, 5, 27), snt::hours(20));
  snt::time_point t = start;
  while (t < stop) {
    double act = ppam.compute_activity(t);

    std::cout << snt::to_quantity(t - start) / CLHEP::second
              << ' ' << act / CLHEP::becquerel
              << std::endl;
    t += snt::minutes(1);
  }
  
  return;
}

void test2()
{
  namespace snt = snemo::time;
  namespace snpm = snemo::physics_model;

  snpm::activity_model_dict_type amDict;
 
  {
    datatools::properties constConfig;
    constConfig.store_real_with_explicit_unit("activity", 2.0 * CLHEP::becquerel);
    constConfig.set_unit_symbol("activity", "Bq");
    {
      auto am = std::make_shared<snpm::constant_activity_model>();
      am->initialize(constConfig, amDict);
      amDict.emplace("model1", am);
    }
    
    datatools::properties scaledConfig;
    scaledConfig.store("scale_factor", 2.5);
    scaledConfig.store("model", "model1");
    {
      auto am = std::make_shared<snpm::scaled_activity_model>();
      am->initialize(scaledConfig, amDict);
      amDict.emplace("model2", am);
    }
    
    datatools::properties slicedConfig;
    slicedConfig.store("slice.period", "{2022-05-27 02:13:42.678000+1:30:45}");
    slicedConfig.store("model", "model2");
    {
      auto am = std::make_shared<snpm::sliced_activity_model>();
      am->initialize(slicedConfig, amDict);
      amDict.emplace("model3", am);
    }
    
  }
  
  auto am1 = snpm::get_model(amDict, "model1");
  auto am2 = snpm::get_model(amDict, "model2");
  auto am3 = snpm::get_model(amDict, "model3");
  
  snt::time_point start(snt::date(2022, 5, 27), snt::hours(1));
  snt::time_point stop(snt::date(2022, 5, 27), snt::hours(4));
  snt::time_point t = start;
  while (t < stop) {
    double a1 = am1->compute_activity(t);
    double a2 = am2->compute_activity(t);
    double a3 = am3->compute_activity(t);
    std::cout << snt::to_quantity(t - start) / CLHEP::second
              << ' ' << a1 / CLHEP::becquerel
              << ' ' << a2 / CLHEP::becquerel
              << ' ' << a3 / CLHEP::becquerel
              << std::endl;
    t += snt::minutes(1);
  }
 
  
  return;
}
