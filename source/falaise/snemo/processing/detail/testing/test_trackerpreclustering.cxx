// falaise/testing/test_trackerpreclustering.cxx

// Standard library:
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

// Third party:
// - CLHEP:
#include <CLHEP/Units/SystemOfUnits.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>

// This project:
#include "event_display.h"
#include "event_generator.h"
#include "falaise/snemo/processing/detail/GeigerTimePartitioner.h"
#include "gg_hit.h"

void wait_for_key();

int main(int argc_, char** argv_) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'TrackerPreClustering' algorithm." << std::endl;

    bool draw = false;
    bool process_prompt = true;
    bool process_delayed = true;
    bool split_chamber = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else if ((option == "-P")) {
          process_prompt = false;
        } else if ((option == "-Y")) {
          process_delayed = false;
        } else if ((option == "-S")) {
          split_chamber = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        { std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl; }
      }
      iarg++;
    }

    // The main pre-clustering algorithm :
    snreco::detail::GeigerTimePartitioner PC(10.0 * CLHEP::microsecond, process_prompt,
                                             process_delayed, split_chamber);

    // Generate fake hits :
    TrackerPreClustering::event_generator EG;
    EG.set_seed(314159);
    EG.initialize();

    // Event display :
    TrackerPreClustering::event_display ED;
    ED.set_cell_size(44. * CLHEP::mm);
    ED.set_nb_layers(9);
    ED.set_nb_rows(113);

    // Event loop :
    for (unsigned int ievent = 0; ievent < 20; ievent++) {
      std::clog << "Processing event #" << ievent << "..." << std::endl;
      typedef TrackerPreClustering::gg_hit hit_type;
      snreco::detail::GeigerHitPtrCollection<hit_type> idata;

      EG.shoot_event(idata);
      std::clog << "NOTICE: Event #" << ievent << " has " << idata.size() << " hits." << std::endl;

      snreco::detail::GeigerHitTimePartition<hit_type> odata = PC.partition(idata);

      if (draw) {
        // all input hits :
        std::string fgghits_name = "__gg_hits.data";
        {
          std::ofstream fgghits(fgghits_name.c_str());
          ED.display_gg_hits<hit_type>(fgghits, idata, TrackerPreClustering::event_display::prompt);
          fgghits << std::endl << std::endl;
          ED.display_gg_hits<hit_type>(fgghits, idata,
                                       TrackerPreClustering::event_display::delayed);
          fgghits << std::endl << std::endl;
        }
        // ignored hits :
        std::string figghits_name = "__igg_hits.data";
        {
          std::ofstream figghits(figghits_name.c_str());
          ED.display_cluster<hit_type>(figghits, odata.ignoredHits,
                                       TrackerPreClustering::event_display::prompt |
                                           TrackerPreClustering::event_display::delayed);
        }
        // time-clusters of prompt hits :
        std::string fpcluster_name = "__pcluster.data";
        {
          std::ofstream fpcluster(fpcluster_name.c_str());
          for (unsigned int i = 0; i < odata.promptClusters.size(); i++) {
            ED.display_cluster<hit_type>(fpcluster, odata.promptClusters[i],
                                         TrackerPreClustering::event_display::prompt);
            fpcluster << std::endl << std::endl;
          }
        }
        // time-clusters of delayed hits :
        std::string fdcluster_name = "__dcluster.data";
        {
          std::ofstream fdcluster(fdcluster_name.c_str());
          for (unsigned int i = 0; i < odata.delayedClusters.size(); i++) {
            ED.display_cluster<hit_type>(fdcluster, odata.delayedClusters[i],
                                         TrackerPreClustering::event_display::delayed);
            fdcluster << std::endl << std::endl;
          }
        }
        std::string fdisplaygp_name = "__display.gp";
        std::ofstream fdisplaygp(fdisplaygp_name.c_str());
        std::ostringstream gp_command;
        gp_command << "set output\n";
        gp_command << "set terminal x11\n";
        gp_command << "set xlabel \"x\" \n";
        gp_command << "set ylabel \"y\" \n";
        gp_command << "set zlabel \"z\" \n";
        gp_command << "set grid \n";
        gp_command << "set size ratio -1 \n";
        gp_command << "set arrow 1 from 0,-3000 to 0,3000 nohead lt -1 lw 1  \n";
        gp_command << "set key on outside right top vertical box \n";
        gp_command << "set xrange [-500:500] \n";
        gp_command << "set yrange [-3000:3000] \n";
        gp_command << "set zrange [-2000:2000] \n";
        gp_command << "plot ";
        gp_command << "'" << fgghits_name << "' index 0 title 'prompt hits' with lines lt 3";
        gp_command << ", '" << fgghits_name << "' index 1 title 'delayed hits' with lines lt 4";
        if (odata.ignoredHits.size() > 0) {
          gp_command << ", '" << figghits_name << "' index 0 title 'ignored hits' with lines lt 7";
        }
        for (unsigned int i = 0; i < odata.promptClusters.size(); i++) {
          gp_command << ", '" << fpcluster_name << "' index " << i
                     << " title 'prompt time cluster #" << i << "' with lines lt " << (2 + i);
        }
        for (unsigned int i = 0; i < odata.delayedClusters.size(); i++) {
          gp_command << ", '" << fdcluster_name << "' index " << i
                     << " title 'delayed time cluster #" << i << "' with lines lt "
                     << (2 + odata.promptClusters.size() + i);
        }
        gp_command << "\n";
        gp_command << "pause -1 'Hit [Enter]...' ;\n";
        gp_command << std::flush;

        Gnuplot g1("lines");
        g1.cmd(gp_command.str());
        g1.showonscreen();
        wait_for_key();
        unlink(fdisplaygp_name.c_str());
        unlink(fgghits_name.c_str());
        unlink(figghits_name.c_str());
        unlink(fpcluster_name.c_str());
        unlink(fdcluster_name.c_str());
      }
    }

    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "ERROR: "
              << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void wait_for_key() {
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
  std::clog << std::endl << "Press ENTER to continue..." << std::endl;
  std::cin.clear();
  std::cin.ignore(std::cin.rdbuf()->in_avail());
  std::cin.get();
#endif
  return;
}
