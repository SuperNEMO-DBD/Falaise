// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <lttc/sngeometry.hh>
#include <lttc/track_simulator.hh>
#include <lttc/track.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);

    lttc::tracker sntracker;
    lttc::tracker_drawer trackerDrawer(sntracker);
    trackerDrawer.draw(std::cout);
    std::cout << '\n';
    std::cout << '\n';

    lttc::track_simulator::config trackSimCfg;
    trackSimCfg.logging = datatools::logger::PRIO_DEBUG;
    trackSimCfg.track_shape = lttc::track_simulator::TRACK_SHAPE_LINE;
    trackSimCfg.track_shape = lttc::track_simulator::TRACK_SHAPE_CIRCLE;
    trackSimCfg.kinked_trajectory = true;
    trackSimCfg.kinked_prob = 1.0;

    lttc::track_simulator trackSim(sntracker, trackSimCfg);
    
    lttc::track track1;
    trackSim.shoot(generator, track1);
    std::cout << "#@track1" << '\n';
    lttc::draw_polyline(std::cout, track1.pl);
    std::cout << '\n';
    std::cout << '\n';
    
    lttc::track track2;
    trackSim.shoot(generator, track2);
    std::cout << "#@track2" << '\n';
    lttc::draw_polyline(std::cout, track2.pl);
    std::cout << '\n';
    
    std::cout << '\n';
    
    // lttc::hit_simulator::config hitSimConfig;
    // hitSimConfig.debug = true;
    // hitSimConfig.drift_radius_err = 0.15;
    // hitSimConfig.add_noisy_hits = true;
    // hitSimConfig.nb_noisy_hits = 3;
    // lttc::hit_simulator hitSim(hitSimConfig);
    // lttc::hit_collection hits;
    // hitSim.generate_hits(generator, track1, hits);
    // hitSim.generate_hits(generator, track2, hits);
    // for (const auto & h : hits) {
    //   std::cout << "#@hit\n";
    //   h.draw(std::cout);
    // }
    // std::cout << '\n';
   
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
