// falaise/testing/test_trackerpreclustering.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <unistd.h>

// Third party:
// - CLHEP:
#include <CLHEP/Units/SystemOfUnits.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>

// This project:
#include <TrackerPreClustering/pre_clusterizer.h>
#include <TrackerPreClustering/gg_hit.h>
#include <TrackerPreClustering/event_generator.h>
#include <TrackerPreClustering/event_display.h>

void wait_for_key();

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'TrackerPreClustering' algorithm." << std::endl;

    bool debug = false;
    bool draw = false;
    bool process_prompt = true;
    bool process_delayed = true;
    bool split_chamber = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-')  {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else if ((option == "-P")) {
          process_prompt = false;
        } else if ((option == "-Y")) {
          process_delayed = false;
        } else if ((option == "-S")) {
          split_chamber = true;
        } else {
          std::clog << "warning: ignoring option '"
                    << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '"
                    << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    // The main pre-clustering algorithm :
    TrackerPreClustering::pre_clusterizer PC;

    // The algorithm's configuration parameters :
    TrackerPreClustering::setup_data PC_config;
    PC_config.debug = debug;
    PC_config.cell_size                = 44.0 * CLHEP::mm;
    PC_config.delayed_hit_cluster_time = 10.0 * CLHEP::microsecond;
    PC_config.processing_prompt_hits   = process_prompt;
    PC_config.processing_delayed_hits  = process_delayed;
    PC_config.split_chamber            = split_chamber;

    // Check :
    if (! PC_config.check()) {
      std::ostringstream message;
      message << "TrackerPreClustering: "
              << "Invalid setup data : "
              << PC_config.get_last_error_message();
      throw std::logic_error(message.str());
    }

    // Configure the algorithm :
    PC.initialize(PC_config);

    // Generate fake hits :
    TrackerPreClustering::event_generator EG;
    EG.set_seed(314159);
    EG.initialize();

    // Event display :
    TrackerPreClustering::event_display   ED;
    ED.set_cell_size(44. * CLHEP::mm);
    ED.set_nb_layers(9);
    ED.set_nb_rows(113);

    // Event loop :
    for (unsigned int ievent = 0; ievent < 20; ievent++) {
      std::clog << "Processing event #" << ievent << "..." << std::endl;
      typedef TrackerPreClustering::gg_hit hit_type;
      TrackerPreClustering::input_data<hit_type>  idata;
      TrackerPreClustering::output_data<hit_type> odata;

      EG.shoot_event(idata.hits);
      std::clog << "NOTICE: Event #"
                << ievent << " has " << idata.hits.size() << " hits."
                << std::endl;
      if (! idata.check()) {
        std::clog << "ERROR: Invalid pre-clustering input data for event #"
                  << ievent << ": " << idata.get_last_error_message() << std::endl;
        continue;
      }

      int status = PC.process<hit_type>(idata, odata);
      if (status != TrackerPreClustering::pre_clusterizer::OK) {
        std::clog << "ERROR: Pre-clustering failed for event #"
                  << ievent << "! " << std::endl;
      }

      odata.dump(std::clog);

      if (draw) {
        // all input hits :
        std::string fgghits_name = "__gg_hits.data";
        {
          std::ofstream fgghits(fgghits_name.c_str());
          ED.display_gg_hits<hit_type>(fgghits,
                                       idata.hits,
                                       TrackerPreClustering::event_display::prompt);
          fgghits << std::endl << std::endl;
          ED.display_gg_hits<hit_type>(fgghits,
                                       idata.hits,
                                       TrackerPreClustering::event_display::delayed);
          fgghits << std::endl << std::endl;
        }
        // ignored hits :
        std::string figghits_name = "__igg_hits.data";
        {
          std::ofstream figghits(figghits_name.c_str());
          ED.display_cluster<hit_type>(figghits,
                                       odata.ignored_hits,
                                       TrackerPreClustering::event_display::prompt | TrackerPreClustering::event_display::delayed);
        }
        // time-clusters of prompt hits :
        std::string fpcluster_name = "__pcluster.data";
        {
          std::ofstream fpcluster(fpcluster_name.c_str());
          for (unsigned int i = 0; i < odata.prompt_clusters.size(); i++) {
            ED.display_cluster<hit_type>(fpcluster,
                                         odata.prompt_clusters[i],
                                         TrackerPreClustering::event_display::prompt);
            fpcluster << std::endl << std::endl;
          }
        }
        // time-clusters of delayed hits :
        std::string fdcluster_name = "__dcluster.data";
        {
          std::ofstream fdcluster(fdcluster_name.c_str());
          for (unsigned int i = 0; i < odata.delayed_clusters.size(); i++) {
            ED.display_cluster<hit_type>(fdcluster,
                                         odata.delayed_clusters[i],
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
        if(odata.ignored_hits.size() > 0) {
          gp_command << ", '" << figghits_name << "' index 0 title 'ignored hits' with lines lt 7";
        }
        for (unsigned int i = 0; i < odata.prompt_clusters.size(); i++) {
          gp_command << ", '" << fpcluster_name << "' index " << i << " title 'prompt time cluster #" << i << "' with lines lt " <<(2 + i);
        }
        for (unsigned int i = 0; i < odata.delayed_clusters.size(); i++) {
          gp_command << ", '" << fdcluster_name << "' index " << i << " title 'delayed time cluster #" << i << "' with lines lt " <<(2 + odata.prompt_clusters.size() + i);
        }
        gp_command << "\n";
        gp_command << "pause -1 'Hit [Enter]...' ;\n";
        gp_command << std::flush;

        Gnuplot g1("lines");
        g1.cmd(gp_command.str());
        g1.showonscreen();
        wait_for_key();
        // std::ostringstream cmd;
        // cmd << TRACKERPRECLUSTERING_GNUPLOT_EXECUTABLE << ' ' << "__display.gp";
        // if(debug) {
        //   std::cerr << "DEBUG: system : '" << cmd.str() << "'" << std::endl;
        // }
        // int ret = system(cmd.str().c_str());
        // if(ret != 0) {
        //   std::cerr << "WARNING: Cannot draw the event !" << std::endl;
        //   draw = false;
        // }
        unlink(fdisplaygp_name.c_str());
        unlink(fgghits_name.c_str());
        unlink(figghits_name.c_str());
        unlink(fpcluster_name.c_str());
        unlink(fdcluster_name.c_str());
      }
    }

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "ERROR: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void wait_for_key()
{
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
  std::clog << std::endl << "Press ENTER to continue..." << std::endl;
  std::cin.clear();
  std::cin.ignore(std::cin.rdbuf()->in_avail());
  std::cin.get();
#endif
  return;
}
