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
#include <lttc/hit_simulator.hh>
#include <lttc/tracker_hit.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // seed = 31415912;
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
    trackSimCfg.kinked_prob = 0.75;

    lttc::track_simulator trackSim(sntracker, trackSimCfg);

    std::vector<lttc::track> tracks;
    int ntracks = 3;
    for (int i = 0; i < ntracks; i++) {
      lttc::track track;
      track.id = i;
      trackSim.shoot(generator, track);
      std::cout << "#@track-" << i << '\n';
      lttc::draw_polyline(std::cout, track.pl);
      std::cout << '\n';
      tracks.push_back(track);
    }
    std::clog << "Number of generated tracks = " << tracks.size() << '\n';
 
    lttc::hit_simulator::config hitSimCfg;
    hitSimCfg.logging = datatools::logger::PRIO_DEBUG;
    hitSimCfg.drift_radius_err = 0.15 * CLHEP::mm;
    hitSimCfg.add_noisy_hits = true;
    hitSimCfg.nb_noisy_hits = 5;

    lttc::hit_simulator hitSim(sntracker, hitSimCfg);

    lttc::tracker_hit_collection trackerHits;

    for (const lttc::track & aTrack : tracks) {
      hitSim.generate_hits(generator, aTrack, trackerHits);
    }
    trackerHits.print(std::clog);
    
    std::clog << "Number of hits = " << trackerHits.size() << "\n";
    for (const auto & hit : trackerHits) {
      // std::clog << "Hit #" << (trackerHits.size()-1) << " :\n";
      // hit.print(std::clog);
      lttc::tracker_hit_drawer trkHitDrawer(hit, sntracker);
      trkHitDrawer.draw(std::cout);
    }
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
