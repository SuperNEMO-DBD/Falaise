// test_snemo_rc_run_statistics.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>

// This project:
#include <falaise/snemo/rc/run_statistics.h>
#include <falaise/snemo/physics_model/per_period_activity_model.h>
#include <falaise/snemo/rc/run_list.h>
#include <falaise/snemo/rc/mc_event_distribution.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::rc::run_statistics'!" << std::endl;
    test1();
    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void test1()
{
  std::clog << "\nTest 1:\n";
 
  namespace snrc = snemo::rc;
  namespace snt = snemo::time;
  namespace snpm = snemo::physics_model;

  snrc::run_list runList;
  datatools::multi_properties runListConfig("run", "type");
  std::string runListConfigFilename
    = "${FALAISE_SNEMO_TESTING_DIR}/config/test-run_list1.conf";
  datatools::fetch_path_with_env(runListConfigFilename);
  runListConfig.read(runListConfigFilename);
  runListConfig.print_tree(std::clog);
  runList.load(runListConfig);
  std::clog << "\nRun list:\n";
  runList.print_tree(std::clog);
  
  snpm::per_period_activity_model activityModel;
  datatools::properties activityModelConfig;
  std::string activityModelConfigFilename
    = "${FALAISE_SNEMO_TESTING_DIR}/config/test-per_period_activity_model.conf";
  datatools::fetch_path_with_env(activityModelConfigFilename);
  activityModelConfig.read_configuration(activityModelConfigFilename);
  activityModel.initialize_standalone(activityModelConfig);
  std::clog << "\nActivity model:\n";
  activityModel.print_tree(std::clog);
 
  snt::time_duration timeStep = snt::seconds(5);
  snrc::run_statistics runStats;
  runStats.set_runs(runList);
  runStats.set_activity_model(activityModel);
  runStats.set_time_step(timeStep);
  runStats.initialize();
  std::clog << "\nRun statitics:\n";
  runStats.print_tree(std::clog);

  snt::time_point tStart = snt::invalid_point();
  snt::time_point tStop = snt::invalid_point();
  const auto & runRecords = runStats.records();
  for (const auto & runRec : runRecords) {
    auto runId = runRec.run_id;
    std::string foutName = "run_decay_dist-" + std::to_string(runId) + ".data";
    std::ofstream fout(foutName);
    const auto & runDesc = runStats.runs().get_run(runId);
    if (not snt::is_valid(tStart)) {
      tStart = runDesc.begin();
    }
    tStop = runDesc.end();
    fout << "#@run-id=" << runId << '\n';
    double runTimeStart = snt::to_quantity(runDesc.begin() - tStart);
    fout << "#@time-start=" << runTimeStart << '\n';
    for (mygsl::tabulated_function::points_map_type::const_iterator i
           = runRec.cumul_dist.points().begin();
         i != runRec.cumul_dist.points().end();
         ++i) {
      fout << (i->first + runTimeStart) / CLHEP::microsecond << ' ' << i->second << std::endl;
    }
    fout.close();
  }

  unsigned int nbDecays = 1000000;
  for (const auto & runRec : runRecords) {
    auto runId = runRec.run_id;
    snrc::run_statistics::inverse_cumul_dist inverseCumul(runRec.cumul_dist);
    std::string foutName = "run_decay_random-" + std::to_string(runId) + ".data";
    std::ofstream fout(foutName);
    unsigned int runNbDecays = (unsigned int) (nbDecays * runRec.probability);
    fout << "#@nb-decays-ref=" << runStats.runs().get_run(runId).number_of_events() << '\n';
    fout << "#@nb-decays=" << nbDecays << '\n';
    snt::time_point runStart = runStats.runs().get_run(runId).begin();
    fout << "#@time-start=" << snt::to_quantity(runStart - tStart) / CLHEP::microsecond << '\n';
    fout << "#@time-min=" << (runRec.cumul_dist.x_min() + snt::to_quantity(runStart - tStart)) / CLHEP::microsecond << '\n';
    fout << "#@time-max=" << (runRec.cumul_dist.x_max() + snt::to_quantity(runStart - tStart)) / CLHEP::microsecond << '\n';
    for (unsigned int iDecay = 0; iDecay < runNbDecays; iDecay++) {
      double p = drand48();
      double t = inverseCumul(p) / CLHEP::microsecond ;
      fout << t << '\n';
    }
    fout.close();
  }
  
  {
    std::string foutName = "activity.data";
    std::ofstream fout(foutName);
    snt::time_point t = tStart;
    snt::time_duration dt = snt::seconds(5);
    while (t < tStop) {
      double act = activityModel.compute_activity(t);
      fout << snt::to_quantity(t - tStart) / CLHEP::microsecond << ' ' << act / CLHEP::becquerel << std::endl;
      t += dt;
    }
    fout << "\n\n";
    
    snt::time_point t_begin = activityModel.time_span().begin();
    snt::time_point t_end = activityModel.time_span().end();
    t = t_begin;
    while (t < t_end) {
      double act = activityModel.compute_activity(t);
      fout << snt::to_quantity(t - tStart) / CLHEP::microsecond << ' ' << act / CLHEP::becquerel << std::endl;
      t += dt;
    }
    fout << "\n\n";
    fout.close();
  }

  
  std::uint32_t nbEvents = 10000;
  long randomSeed = 3141591;
  randomSeed = 0;
  datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
  snrc::mc_event_distribution mcEventDist(runStats, nbEvents, randomSeed, logging);
  std::clog << "\nMC event distribution: " << '\n';
  mcEventDist.print_tree(std::clog);
  std::clog << mcEventDist.has_next_decay() << '\n';
  // while (mcEventDist.has_next_decay()) {
  //   auto decay = mcEventDist.next_decay();
  //   std::int32_t runId = decay.run_id;
  //   snt::time_point decayTime = decay.decay_time;
  //   std::clog << "Run #" << runId << " DecayTime=" << snt::to_string(decayTime) << '\n';
  // }
  
  return;
}
