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
#include <lttc/lttc_algo.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // seed = 31415926;
    // seed = 3141592;
    seed = 2608366040;
    std::clog << "seed=" << seed << '\n';
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);

    lttc::tracker sntracker;
    {
      lttc::tracker_drawer trackerDrawer(sntracker);
      std::ofstream ftrackergeom("test-lttc_algo-tracker-geometry.data");
      trackerDrawer.draw(ftrackergeom);
      std::cout << '\n';
      std::cout << '\n';
    } 
     
    lttc::tracker_conditions trackerConds;
    trackerConds.add_dead_cell(lttc::cell_id(1, 0, 0));
    trackerConds.add_dead_cell(lttc::cell_id(1, 1, 0));
    trackerConds.add_dead_cell(lttc::cell_id(1, 2, 0));
    trackerConds.add_dead_cell(lttc::cell_id(1, 3, 22));
    trackerConds.add_dead_cell(lttc::cell_id(1, 4, 22));
    trackerConds.add_dead_cell(lttc::cell_id(1, 7, 34));
    trackerConds.add_dead_cell(lttc::cell_id(1, 3, 51));
    trackerConds.add_dead_cell(lttc::cell_id(0, 0, 61));
    trackerConds.add_dead_cell(lttc::cell_id(0, 5, 61));
    trackerConds.add_dead_cell(lttc::cell_id(1, 6, 62));
    {
      std::ofstream ftrackerdcells("test-lttc_algo-tracker-dead-cells.data");
      ftrackerdcells << "#@tracker-dead-cells\n";
      lttc::tracker_conditions_drawer trackerCondsDrawer(trackerConds, sntracker);
      trackerCondsDrawer.draw(ftrackerdcells);
      ftrackerdcells << '\n';
    }
    sntracker.set_tracker_conditions(trackerConds);
    
    lttc::track_simulator::config trackSimCfg;
    // trackSimCfg.logging = datatools::logger::PRIO_DEBUG;
    trackSimCfg.track_shape = lttc::track_simulator::TRACK_SHAPE_LINE;
    // trackSimCfg.track_shape = lttc::track_simulator::TRACK_SHAPE_CIRCLE;
    // trackSimCfg.kinked_trajectory = true;
    // trackSimCfg.kinked_prob = 0.75;

    lttc::track_simulator trackSim(sntracker, trackSimCfg);
    std::vector<lttc::track> tracks;
    {
      int ntracks = 3;
      std::ofstream ftracks("test-lttc_algo-tracks.data");
      for (int i = 0; i < ntracks; i++) {
        lttc::track track;
        track.id = i;
        trackSim.shoot(generator, track);
        ftracks << "#@track-" << i << '\n';
        lttc::draw_polyline(ftracks, track.pl);
        ftracks << '\n';
        tracks.push_back(track);
      }
      ftracks << '\n';
    }
    std::clog << "Number of generated tracks = " << tracks.size() << '\n';

    lttc::hit_simulator::config hitSimCfg;
    // hitSimCfg.logging = datatools::logger::PRIO_DEBUG;
    hitSimCfg.drift_radius_err = 0.15 * CLHEP::mm;
    hitSimCfg.add_noisy_hits = false;
    hitSimCfg.nb_noisy_hits = 5;

    lttc::hit_simulator hitSim(sntracker, hitSimCfg);
    hitSim.set_trk_conds(trackerConds);
    lttc::lttc_algo::input_data lttcInData; 
    for (const auto & aTrack : tracks) {
      hitSim.generate_hits(generator, aTrack, lttcInData.hits);
    }
    hitSim.generate_noisy_hits(generator, lttcInData.hits);
    std::clog << "Number of hits = " << lttcInData.hits.size() << "\n";
    {
      std::ofstream fhits("test-lttc_algo-hits.data");
      for (const auto & aHit : lttcInData.hits) {
        lttc::tracker_hit_drawer trkHitDrawer(aHit, sntracker);
        trkHitDrawer.draw(fhits);
      }
      fhits << '\n';
    }   
    
    lttc::lttc_algo::config lttcAlgoCfg;
    lttcAlgoCfg.logging = datatools::logger::PRIO_DEBUG;
    lttcAlgoCfg.mode = lttc::lttc_algo::MODE_LINE;
    lttcAlgoCfg.draw = true;
    lttcAlgoCfg.draw_prefix = "test-lttc_algo-";
    // lttcAlgoCfg.step1_ntbins = 300;
    // lttcAlgoCfg.step1_nrbins = 300;
    lttcAlgoCfg.step1_track_threshold = 3.0;
    lttcAlgoCfg.step2_max_nlines = 6;
    lttcAlgoCfg.step2_delta_theta = 1.5e-3;
    lttcAlgoCfg.step2_delta_r = 0.2;
    lttcAlgoCfg.step2_gauss_threshold = 0.05;
    lttcAlgoCfg.step2_track_threshold = 3.0;
    lttcAlgoCfg.step3_nsigma = 3.0;
    lttcAlgoCfg.step3_nsigma_outliers = 2.0;
   
    lttc::lttc_algo lttcAlgo(sntracker, lttcAlgoCfg);

    lttc::lttc_algo::output_data lttcOutData;
    lttcAlgo.process(lttcInData, lttcOutData);
    
    std::clog << "\nThe end.\n";
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
