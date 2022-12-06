//! \file    flsimrc_timestampermain.cxx
// Copyright (c) 2022 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2022 by Université de Caen Normandie
//
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise.  If not, see <http://www.gnu.org/licenses/>.

// Standard library:
#include <string>
#include <fstream>
#include <memory>
#include <exception>

// Third Party:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/library_loader.h>
#include <bayeux/datatools/utils.h>

// This Project
#include "falaise/exitcodes.h"
#include "falaise/falaise.h"
#include "falaise/resource.h"
#include "falaise/snemo/rc/mc_event_distribution.h"
#include "app_params.hpp"
#include "app_command_line.hpp"
#include "snsimrc/event_timestamper.hpp"
#include "snsimrc/simrc_lib.hpp"

namespace FLSimRC {

  //! Perform MC event timestamping using command line args as given
  falaise::exit_code do_mc_event_timestamping(int argc_, char* argv_[]);
  
  void do_configure(int argc_, char* argv_[], app_params & params_);
  
  void do_run(const app_params & params_);

}  // end of namespace FLSimRC


//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc_, char * argv_[])
{
  falaise::initialize(argc_, argv_);
  snsimrc::simrc_initialize();
  
  // - Do the MC event timestampîng
  falaise::exit_code ret = FLSimRC::do_mc_event_timestamping(argc_, argv_);

  snsimrc::simrc_terminate();
  falaise::terminate();
  return ret;
}

namespace FLSimRC {

  //! Perform reconstruction using command line args as given
  falaise::exit_code do_mc_event_timestamping(int argc_, char* argv_[])
  {
    app_params appParams;
    try {
      do_configure(argc_, argv_, appParams);
      DT_LOG_DEBUG(appParams.log_level, "flsimrc timestamper is configured.");
    } catch (cl_help_signal & signal) {
      return falaise::EXIT_OK;
    } catch (std::exception & error) {
      std::cerr << "flsimrc-timestamper : User configuration error: " << error.what() << std::endl;
      return falaise::EXIT_ERROR;
    }

    falaise::exit_code code = falaise::EXIT_OK;
    // - Run
    try {
      do_run(appParams);
    } catch (std::exception & error) {
      std::cerr << "flsimrc-timestamper : Run error: " << error.what() << std::endl;
      return falaise::EXIT_ERROR;
    }
    
    return code;
  }

  void do_configure(int argc_, char* argv_[], app_params & params_)
  {
    params_ = app_params::make_default();
    int errorCode = falaise::EXIT_OK;
    try {
      errorCode = do_cldialog(argc_, argv_, params_);
      if (errorCode != falaise::EXIT_OK) {
        throw std::logic_error("bad command line input");
      }
    } catch (cl_help_signal & s) {
      throw s;
    } catch (std::exception & e) {
      throw e;
    }

    if (params_.activity_model_factory_conf_path.empty()) {
      DT_LOG_DEBUG(params_.log_level, "No activity model factory config file");
    } 
 
    if (params_.activity_model_name.empty()) {
      DT_LOG_DEBUG(params_.log_level, "No activity model name!");
    }
  
    if (params_.run_list_path.empty()) {
      DT_LOG_DEBUG(params_.log_level, "No run list file!");
    }

    DT_THROW_IF(params_.number_of_mc_events == 0, std::domain_error,
                "Missing number of MC events!");
        
    return;
  }
 
  void do_run(const app_params & params_)
  {
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_TRACE);
    // datatools::library_loader libLoader;
    // std::string pluginPath = falaise::get_plugin_dir();
    // libLoader.load("Falaise_XXX", pluginPath);
    
    datatools::properties eventTimestamperConfig;
    if (datatools::logger::is_debug(params_.log_level)) {
      eventTimestamperConfig.store("debug", true);
    }
    std::vector<std::string> runListFilenames;
    if (not params_.run_list_path.empty()) {
      runListFilenames.push_back(params_.run_list_path);
    }
    eventTimestamperConfig.store("run_lists", runListFilenames);
    
    if (not params_.activity_model_factory_conf_path.empty()) {
      eventTimestamperConfig.store("activity_model_factory.config",
                                   params_.activity_model_factory_conf_path);
      if (not params_.activity_model_name.empty()) {
        eventTimestamperConfig.store("activity_model",
                                     params_.activity_model_name);
      }
    }

    eventTimestamperConfig.store_integer("number_of_events", params_.number_of_mc_events);
    if (params_.timestamp_generation_random_seed > 0) {
      eventTimestamperConfig.store_integer("random_seed", (int) params_.timestamp_generation_random_seed);
    }
    
    DT_LOG_TRACE(params_.log_level, "Instance of event timestamper");
    snemo::simulation::event_timestamper eventTimestamper;
    eventTimestamper.initialize(eventTimestamperConfig);
    DT_LOG_TRACE(params_.log_level, "... initialized.");

    std::string eventTimestampsPath(params_.event_timestamps_path);
    datatools::fetch_path_with_env(eventTimestampsPath);

    std::unique_ptr<std::ofstream> fTimesOut;
    if (! params_.event_times_path.empty()) {
      DT_LOG_DEBUG(params_.log_level,
                   "Store generated times in file '" << params_.event_times_path << "'");
      std::string eventTimesPath(params_.event_times_path);
      datatools::fetch_path_with_env(eventTimesPath);
      fTimesOut = std::make_unique<std::ofstream>(eventTimesPath);
    }

    snemo::time::time_point refTime = eventTimestamper.run_list().span().begin();
    {
      snemo::rc::mc_event_distribution::writer timestampsWriter(eventTimestampsPath,
                                                                params_.number_of_mc_events);    
      auto & mcEventDistribution = eventTimestamper.mc_event_distribution();
      while (mcEventDistribution.has_next_decay()) {
        auto evenRcTiming = mcEventDistribution.next_decay();
        timestampsWriter.write(evenRcTiming);
        if (fTimesOut){
          *fTimesOut << snemo::time::to_quantity(evenRcTiming.decay_time - refTime) / CLHEP::second << " " << evenRcTiming.run_id << '\n';
        }
      }
    }

    if (fTimesOut) {
      fTimesOut->close();
      fTimesOut.reset();
    }
    
    eventTimestamper.reset();
    return;
  }

} // end of namespace FLSimRC

