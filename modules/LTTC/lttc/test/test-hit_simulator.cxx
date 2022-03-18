// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
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
    std::ofstream fout("test-hit_simulator.data");

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    unsigned zseed = std::chrono::system_clock::now().time_since_epoch().count() + 2131;
    // seed = 31415912;
    // seed=235178786;
    // seed=1922533372;
    // seed=1999351793;
    seed=3907129825;
    zseed=1090539080;

    std::clog << "seed=" << seed << '\n';
    std::clog << "zseed=" << zseed << '\n';
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);

    lttc::tracker sntracker;
    lttc::tracker_drawer trackerDrawer(sntracker);
    trackerDrawer.draw(fout);
    fout << '\n';

    lttc::track_simulator::config trackSimCfg;
    trackSimCfg.logging = datatools::logger::PRIO_DEBUG;
    trackSimCfg.track_shape = lttc::track_simulator::TRACK_SHAPE_LINE;
    // trackSimCfg.track_shape = lttc::track_simulator::TRACK_SHAPE_CIRCLE;
    trackSimCfg.kinked_trajectory = true;
    trackSimCfg.kinked_prob = 0.75;

    lttc::track_simulator trackSim(sntracker, trackSimCfg);
    std::vector<lttc::track2> tracksXy;
    int ntracks = 3;
    for (int i = 0; i < ntracks; i++) {
      lttc::track2 track;
      track.id = i;
      trackSim.shoot(generator, track);
      // fout << "#@track-" << i << '\n';
      // lttc::draw_polyline(fout, track.pl);
      // fout << '\n';
      tracksXy.push_back(track);
    }
    std::clog << "Number of generated tracks = " << tracksXy.size() << '\n';

    std::default_random_engine zgenerator(zseed);
    std::vector<lttc::track3> tracks3d;
    for (int i = 0; i < (int) tracksXy.size(); i++) {
      const lttc::track2 & aTrk2 = tracksXy[i];
      tracks3d.push_back(lttc::track3());
      lttc::track3 & aTrk3 = tracks3d.back();
      trackSim.shoot3(zgenerator, aTrk2, aTrk3);
      std::clog << "[debug] Track 3D :\n";
      aTrk3.print(std::clog, "[debug] ");
      fout << "#@track-" << i << '\n';
      lttc::polyline3 trkPl3;
      aTrk3.make(trkPl3);
      lttc::draw_polyline(fout, trkPl3);
      fout << '\n';
    }
 
    lttc::hit_simulator::config hitSimCfg;
    hitSimCfg.logging = datatools::logger::PRIO_DEBUG;
    hitSimCfg.drift_radius_err = 0.15 * CLHEP::mm;
    hitSimCfg.add_noisy_hits = true;
    hitSimCfg.nb_noisy_hits = 5;

    lttc::hit_simulator hitSim(sntracker, hitSimCfg);

    lttc::tracker_hit_collection trackerHits;

    for (const lttc::track3 & aTrack3 : tracks3d) {
      hitSim.generate_hits(generator, aTrack3, trackerHits);
    }
    trackerHits.print(std::clog);
    
    std::clog << "Number of hits = " << trackerHits.size() << "\n";
    for (const auto & hit : trackerHits) {
      // std::clog << "Hit #" << (trackerHits.size()-1) << " :\n";
      // hit.print(std::clog);
      lttc::tracker_hit_drawer trkHitDrawer(hit, sntracker);
      trkHitDrawer.draw(fout);
      fout << '\n';
    }
    fout << '\n';
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
