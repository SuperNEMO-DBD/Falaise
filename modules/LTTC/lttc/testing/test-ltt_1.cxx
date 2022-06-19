// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <lttc/circle.hh>
#include <lttc/tracker.hh>
#include <lttc/track_simulator.hh>
#include <lttc/track.hh>
#include <lttc/hit.hh>
#include <lttc/hit_simulator.hh>
#include <lttc/event.hh>
#include <lttc/point.hh>
#include <lttc/legendre_transform_tools.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    
    lttc::tracker tracker;
    {
      std::ofstream ftrackergeom("tracker-geometry.data");
      ftrackergeom << "#@tracker-geometry\n";
      tracker.draw(ftrackergeom);
      ftrackergeom << '\n';
    }
    
    lttc::tracker_conditions trackerConds;
    trackerConds.add_dead_cell(lttc::cell_id(0, 0));
    trackerConds.add_dead_cell(lttc::cell_id(1, 0));
    trackerConds.add_dead_cell(lttc::cell_id(2, 0));
    trackerConds.add_dead_cell(lttc::cell_id(3, 22));
    trackerConds.add_dead_cell(lttc::cell_id(4, 22));
    trackerConds.add_dead_cell(lttc::cell_id(7, 34));
    trackerConds.add_dead_cell(lttc::cell_id(3, 51));
    trackerConds.add_dead_cell(lttc::cell_id(0, 61));
    trackerConds.add_dead_cell(lttc::cell_id(5, 61));
    trackerConds.add_dead_cell(lttc::cell_id(6, 62));
    {
      std::ofstream ftrackerdcells("tracker-dead-cells.data");
      ftrackerdcells << "#@tracker-dead-cells\n";
      trackerConds.draw(ftrackerdcells);
      ftrackerdcells << '\n';
    }
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // seed = 1314159266;
    std::clog << "seed" << seed << '\n';
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);

    lttc::event evt;
    
    lttc::track_simulator trackSim(lttc::track_simulator::TRACK_LINE);
    {
      std::ofstream ftracks("tracks.data");
      size_t ntracks = 3;
      for (size_t i = 0; i < ntracks; i++) {
        lttc::track trk;
        trackSim.shoot(generator, trk);
        ftracks << "#@track-" << i << '\n';
        ftracks << trk.pl;
        ftracks << '\n';
        evt.tracks.push_back(trk);
      }
      ftracks << '\n';
    }
    
    lttc::hit_simulator::config hitSimConfig;
    hitSimConfig.debug = false;
    // hitSimConfig.debug = true;
    hitSimConfig.drift_radius_err = 0.15;
    hitSimConfig.add_noisy_hits = false;
    // hitSimConfig.add_noisy_hits = true;
    hitSimConfig.nb_noisy_hits = 10;
    lttc::hit_simulator hitSim(hitSimConfig);
    hitSim.set_trk_conds(trackerConds);
    
    for (const auto & trk : evt.tracks) {
      hitSim.generate_hits(generator, trk, evt.hits);
    }
    hitSim.generate_noisy_hits(generator, evt.hits);
     
    {
      std::ofstream fhits("hits-0.data");
      for (int ihit = 0; ihit < (int) evt.hits.size(); ihit++) {
        const auto & h = evt.hits[ihit];
        fhits << "#@hit-#" << ihit << "\n";
        h.draw(fhits);
      }
      fhits << '\n';
    }
      
    lttc::ltc_algo::config ltcAlgoCfg;
    ltcAlgoCfg.debug = true;
    // ltcAlgoCfg.debug = false;
    ltcAlgoCfg.step1_ntbins = 300;
    ltcAlgoCfg.step1_nrbins = 300;
    ltcAlgoCfg.step1_track_threshold = 3.0;
    ltcAlgoCfg.step2_max_nlines = 3;
    ltcAlgoCfg.step2_delta_theta = 1.5e-3;
    ltcAlgoCfg.step2_delta_r = 0.2;
    ltcAlgoCfg.step2_gauss_threshold = 0.05;
    ltcAlgoCfg.step2_track_threshold = 3.0;
    ltcAlgoCfg.step3_nsigma = 10.0;
   
    lttc::ltc_algo ltcAlgo(evt, ltcAlgoCfg);
    
    ltcAlgo.init_loops();
    
    int nloops = 0;
    bool go_loops = true;
    while (go_loops) {

      size_t old_nclusters = ltcAlgo.hit_clustering.clusters.size();
      // if (nloops == 1) {
      //   lttc::ltc_algo::map_type::fill_bin_ctrl::debug = true;
      // }
      
      {
        std::ofstream fhits("hits.data");
        for (int ihit = 0; ihit < (int) evt.hits.size(); ihit++) {
          const auto & h = evt.hits[ihit];
          if (!ltcAlgo.can_process_hit(ihit)) {
            continue;
          }
          fhits << "#@hit\n";
          h.draw(fhits);
        }
        fhits << '\n';
      }
    
      {
        std::clog << "New loop #" << nloops << std::endl;
        // std::string resp;
        // std::getline(std::cin, resp);
      }
    
      std::clog << "Running loop... \n";
      ltcAlgo.run_loop();
      std::clog << "Done.\n";
      
      size_t new_nclusters = ltcAlgo.hit_clustering.clusters.size();
      if ((new_nclusters == old_nclusters) or (ltcAlgo.hit_clustering.unclustered_hits.size() < 3)) {
        go_loops = false;
      }
      const lttc::ltc_algo::loop_data & last_loop = ltcAlgo.loops.back();
      bool print_it = false;

      if (print_it) {
        std::clog << "Step-1: Map:\n";
        last_loop.step1.trmap.print(std::clog);
      }
      
      {
        std::ofstream fstep1bins("step1-bins.data");   
        fstep1bins << "#@step1-bins=" << last_loop.step1.trmap.bins.size() << "\n";
        last_loop.step1.trmap.draw_bins(fstep1bins);
        fstep1bins << '\n';
      }
    
      if (print_it) {
        for (int iSbin = 0; iSbin < (int) last_loop.step1.trmap.sorted_bins.size(); iSbin++) {
          int idxBin = last_loop.step1.trmap.sorted_bins[iSbin];
          int iBin, jBin;
          last_loop.step1.trmap.tr_bin_index_to_t_r_indexes(idxBin, iBin, jBin);
          std::clog << "Sorted bin #" << iSbin << " => bin #" << idxBin
                    << " @(" << iBin << ',' << jBin << ") =>height="
                    << last_loop.step1.trmap.bins[idxBin] << std::endl;
          if (iSbin > 10) {
            std::clog << "..." << '\n';
            break;
          }
        }
      }
    
      {
        std::ofstream fstep1sbins("step1-sorted-bins.data");   
        fstep1sbins << "#@step1-sorted-bins=" << last_loop.step1.trmap.sorted_bins.size() << "\n";
        last_loop.step1.trmap.draw_sorted_bins(fstep1sbins, 0.0);
        fstep1sbins << '\n';
      }
    
      if (print_it) {
        std::clog << "\nStep-1 : Clustering data:\n";
        last_loop.step1.clustering.print(std::clog, "", last_loop.step1.trmap);
        std::clog << '\n';
      }
      
      {
        std::ofstream fstep1clustering("step1-clustering.data");
        last_loop.step1.clustering.draw(fstep1clustering, last_loop.step1.trmap);
        fstep1clustering << '\n';
      }

      {
        std::ofstream fstep2sbins("step2-sorted-bins.data");   
        for (int i = 0; i < (int) last_loop.step2.trmaps.size(); i++) {
          const auto & trmap = last_loop.step2.trmaps[i].trmap;
          fstep2sbins << "#@step2-sorted-bins-cluster[" << i << "]="
                      << trmap.sorted_bins.size() << "\n";
          trmap.draw_sorted_bins(fstep2sbins, 0.0);
          fstep2sbins << '\n';
        }
      }

      {
        std::ofstream fstep2clustering("step2-clustering.data");   
        for (int i = 0; i < (int) last_loop.step2.trmaps.size(); i++) {
          const auto & trmap = last_loop.step2.trmaps[i].trmap;
          const auto & clg = last_loop.step2.trmaps[i].clustering;
          clg.draw(fstep2clustering, trmap);
          fstep2clustering << '\n';
        }
      }

      {
        std::ofstream fstep2lines("step2-lines.data");
        for (int i = 0; i < (int) last_loop.step2.clusters.size(); i++) {
          const auto & trmap = *last_loop.step2.clusters[i].ptrmap;
          const auto & cl    = *last_loop.step2.clusters[i].pcluster;
          const auto & ln    = cl.line_data;
          lttc::polyline ln_samples;
          ln.generate_samples(-500.0, +500.0, ln_samples);
          fstep2lines << "#@step2-lines" << '\n';
          fstep2lines << ln_samples;
          fstep2lines << '\n';
          fstep2lines << '\n';
        }     
      }
 
      std::clog << "Last loop #" << nloops << " : " << '\n';
      last_loop.print(std::clog);
      std::clog << '\n';
      std::clog << "Current hit clustering : \n";
      ltcAlgo.hit_clustering.print(std::cerr, "");
      
      if (ltcAlgoCfg.debug) {
        std::clog << "End of loop #" << nloops << std::endl;
        std::string resp;
        std::getline(std::cin, resp);
      }
      nloops++;

      // if (nloops > 1) break;
    }
   
    ltcAlgo.terminate_loops();
    
    {
      std::ofstream finalHitClustering("hit_clustering.data");
      ltcAlgo.hit_clustering.draw(finalHitClustering, ltcAlgo);
    }
   
    std::clog << "\nThe end.\n";
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
