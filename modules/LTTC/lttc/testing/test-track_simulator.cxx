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

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    std::ofstream fout("test-track_simulator.data");

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    seed = 314159265;
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);

    lttc::tracker sntracker;
    lttc::tracker_drawer trackerDrawer(sntracker);
    trackerDrawer.draw(fout);
    fout << '\n';
    fout << '\n';

    lttc::track_simulator::config trackSimCfg;
    trackSimCfg.logging = datatools::logger::PRIO_DEBUG;
    // trackSimCfg.track_shape = lttc::track_simulator::TRACK_SHAPE_LINE;
    trackSimCfg.track_shape = lttc::track_simulator::TRACK_SHAPE_CIRCLE;
    // trackSimCfg.kinked_trajectory = false;
    trackSimCfg.kinked_trajectory = true;
    trackSimCfg.kinked_prob = 1.0;

    lttc::track_simulator trackSim(sntracker, trackSimCfg);

    std::vector<lttc::track2> tracksXy;
    
    tracksXy.push_back(lttc::track2());
    lttc::track2 & track0 = tracksXy.back();
    trackSim.shoot(generator, track0);
    fout << "#@track0" << '\n';
    lttc::draw_polyline(fout, track0.pl);
    fout << '\n';
    fout << '\n';
    
    tracksXy.push_back(lttc::track2());
    lttc::track2 & track1 = tracksXy.back();
    trackSim.shoot(generator, track1);
    fout << "#@track1" << '\n';
    lttc::draw_polyline(fout, track1.pl);
    fout << '\n';
    fout << '\n';

    unsigned zseed = 314159;
    std::default_random_engine zgenerator(zseed);
    std::vector<lttc::track3> tracks3d;
    for (int i = 0; i < (int) tracksXy.size(); i++) {
      const lttc::track2 & aTrk2 = tracksXy[i];
      tracks3d.push_back(lttc::track3());
      lttc::track3 & aTrk3 = tracks3d.back();
      trackSim.shoot3(zgenerator, aTrk2, aTrk3);
      fout << "#@track" << i << '\n';
      lttc::polyline3 trkPl3;
      aTrk3.make(trkPl3);
      lttc::draw_polyline(fout, trkPl3);
      fout << '\n';
      fout << '\n';
    }
   
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
