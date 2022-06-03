// Ourselves
#include "app_command_line.hpp"

// This project
#include "falaise/version.h"
#include "falaise/exitcodes.h"
#include "falaise/version.h"

namespace FLSimRC {

  //! Handle printing of version information to given ostream
  void do_version(std::ostream & out_)
  {
    std::string commitInfo{};
    if (!falaise::version::get_commit().empty()) {
      commitInfo = " (" + falaise::version::get_commit();
      commitInfo += falaise::version::is_dirty() ? "-dirty" : "";
      commitInfo += ")";
    }   
    out_ << "flsimrc-timestamper " << falaise::version::get_version() << commitInfo << "\n";
  }
  
  void do_help(std::ostream & out_, const boost::program_options::options_description & od_)
  {
    out_ << "flsimrc-timestamper (" << falaise::version::get_version()
       << ") : SuperNEMO timestamper for simulated events\n";
    out_ << "Usage:\n"
       << "  flsimrc-timestamper [options]\n"
       << od_ << "\n";
  }
  
  int do_cldialog(int argc_, char * argv_[], app_params & params_)
  {
    namespace bpo = boost::program_options;
    params_ = app_params::make_default();
    std::string verbosityLabel;
    // Application specific options:
    // clang-format off
    bpo::options_description optDesc("Options");
    optDesc.add_options()
      ("help,h", "print this help message")
      
      ("version", "print version number")

      ("verbosity,V", bpo::value<std::string>(&verbosityLabel)->value_name("level"),
       "set the verbosity level")

      ("activity-model-factory-config,f",
       bpo::value<std::string>(&params_.activity_model_factory_conf_path)->value_name("file"),
       "configuration file of the activity model factory")

      ("activity-model,a",
       bpo::value<std::string>(&params_.activity_model_name)->value_name("name"),
       "selected the activity model from the factory")

      ("run-list,r",
       bpo::value<std::string>(&params_.run_list_path)->value_name("name"),
       "run list definition file")

      ("number-of-events,n",
       bpo::value<uint32_t>(&params_.number_of_mc_events)->value_name("number"),
       "number of MC events to be timestamped (integer > 0)")

      ("event-timestamps-file,o",
       bpo::value<std::string>(&params_.event_timestamps_path)->value_name("file"),
       "file in which to store MC events' generated timestamps")

      ("event-times-file,q",
       bpo::value<std::string>(&params_.event_times_path)->value_name("file"),
       "file in which to store generated time quantities")

      ("timestamp-generation-mode,g",
       bpo::value<std::string>(&params_.timestamp_generation_mode)
       ->value_name("label")
       ->default_value("regular"),
       "timestamp generation mode (supported: 'regular', 'random'")

      ("timestamp-generation-random-seed,s",
       bpo::value<unsigned long>(&params_.timestamp_generation_random_seed)->value_name("number"),
       "seed for the timestamp random generation mode (integer >0)")
      
    ;
    // clang-format on

    // - Store first, handling parse errors
    bpo::variables_map vMap;
    try {
      bpo::store(bpo::parse_command_line(argc_, argv_, optDesc), vMap);
      bpo::notify(vMap);
    } catch (const bpo::required_option & e) {
      // We need to handle help/version even if required_option thrown
      if ((vMap.count("help") == 0u)) {
        std::cerr << "flsimrc : " << e.what() << "\n";
        return falaise::EXIT_OK;
      }
    } catch (const std::exception & e) {
      std::cerr << "flsimrc : " << e.what() << "\n";
      return falaise::EXIT_ERROR;
    }

    // Handle any non-bound options
    if (vMap.count("help") != 0u) {
      do_help(std::cout, optDesc);
      return falaise::EXIT_OK;
    }

    if (vMap.count("version") != 0u) {
      do_version(std::cout);
      return falaise::EXIT_OK;
    }

    if (vMap.count("verbosity") != 0u) {
      params_.log_level = datatools::logger::get_priority(verbosityLabel);
      if (params_.log_level == datatools::logger::PRIO_UNDEFINED) {
        std::cerr << "flsimrc-timestamper : " << "Invalid verbosity level '" + verbosityLabel + "'!\n";
        return falaise::EXIT_ERROR;
      }
    }
    
    if (vMap.count("timestamp-generation-mode") != 0u) {
      if (params_.timestamp_generation_mode != "random"
          and params_.timestamp_generation_mode != "regular") {
        std::cerr << "flsimrc-timestamper : " << "Invalid timestamp generation mode '" + params_.timestamp_generation_mode + "'!\n";
        return falaise::EXIT_ERROR;
      }
    } else {
      params_.timestamp_generation_mode = "regular";
    }

    if (params_.timestamp_generation_mode == "random") {
      if (vMap.count("timestamp-generation-random-seed") == 0u) {
        std::cerr << "flsimrc-timestamper : " << "Missing timestamp generation random seed!\n";
        return falaise::EXIT_ERROR;
      } else {
        if (params_.timestamp_generation_random_seed == 0) {
          std::cerr << "flsimrc-timestamper : " << "Invalid timestamp generation random seed [=0]!\n";
          return falaise::EXIT_ERROR;
        }
      }
    }

    if (vMap.count("event-timestamps-file") == 0u) {
       std::cerr << "flsimrc-timestamper : " << "Missing event timestamps output file name!\n";
       return falaise::EXIT_ERROR;
    } else {
      if (params_.event_timestamps_path.empty()) {
        std::cerr << "flsimrc-timestamper : " << "Empty event timestamps output file name!\n";
        return falaise::EXIT_ERROR;
      }
    }
    
    return falaise::EXIT_OK;
  }

} // namespace FLSimRC
