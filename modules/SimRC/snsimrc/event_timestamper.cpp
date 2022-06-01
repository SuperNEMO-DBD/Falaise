// falaise/snemo/simulation/event_timestamper.cc

// Ourselves:
#include "event_timestamper.hpp"

// Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/clhep_units.h>
// Falaise:
#include <falaise/snemo/physics_model/constant_activity_model.h>

namespace snemo {

  namespace simulation {

    event_timestamper::event_timestamper()
    {
      return;
    }

    bool event_timestamper::is_initialized() const
    {
      return _initialized_;
    }

    void event_timestamper::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Event timestamper is already initialized!");
      
      if (config_.has_flag("debug")) {
        _logging_ = datatools::logger::PRIO_DEBUG;
      }
      
      std::vector<std::string> runListFilenames;
      if (config_.has_key("run_lists")) {
        config_.fetch("run_lists", runListFilenames);
      }
      _runList_ = std::make_unique<snemo::rc::run_list>();
      if (! runListFilenames.empty()) {
        for (std::string runListFilename : runListFilenames) {
          datatools::fetch_path_with_env(runListFilename);
          DT_LOG_DEBUG(_logging_, "Loading run list from '" << runListFilename << "'...");
          datatools::multi_properties runListCfg("run", "type");
          runListCfg.read(runListFilename);
          _runList_->load(runListCfg);
        }
      }
      if (_runList_->is_empty()) {
        DT_LOG_DEBUG(_logging_, "Falling back to an unique long 30-months run...");
        snemo::rc::run_description longRunDesc
          = snemo::rc::run_description::make(0,
                                             snemo::rc::run_category::PRODUCTION,
                                             time::time_period_from_string("[2023-01-01 00:00:00/2025-07-01 00:00:00)"),
                                             1000000);
        _runList_->add_run(longRunDesc);
      }

      std::string activityModelFactoryConfigFilename;
      std::vector<std::string> activityModelFilenames;
      if (config_.has_key("activity_model_factory.config")) {
        activityModelFactoryConfigFilename = config_.fetch_string("activity_model_factory.config");
        datatools::fetch_path_with_env(activityModelFactoryConfigFilename);
        DT_LOG_DEBUG(_logging_, "Configuring activity model factory from '" << activityModelFactoryConfigFilename << "'...");
        datatools::properties activityModelFactoryConfig;
        activityModelFactoryConfig.read_configuration(activityModelFactoryConfigFilename);
        _actModelFactory_.initialize(activityModelFactoryConfig);

        std::string activityModelName;
        if (config_.has_key("activity_model")) {
          activityModelName = config_.fetch_string("activity_model");
        }

        if (not activityModelName.empty()) {
          DT_LOG_DEBUG(_logging_, "Selecting activity model '" << activityModelName << "'...");
          DT_THROW_IF(not _actModelFactory_.has_model(activityModelName), std::logic_error,
                      "No activity model '" << activityModelName << "' is available from the factory!");
          _mcActivityModel_ = _actModelFactory_.get_model_handle(activityModelName);
        }
      }

      if (! _mcActivityModel_) {
        DT_LOG_DEBUG(_logging_, "Falling back to constant activity model...");
        _mcActivityModel_ = std::make_shared<snemo::physics_model::constant_activity_model>();
        dynamic_cast<snemo::physics_model::constant_activity_model*>(_mcActivityModel_.get())->set_activity(10e-3 * CLHEP::becquerel);      
        datatools::properties dummyCfg;
        snemo::physics_model::activity_model_dict_type dummyDict;
        _mcActivityModel_->initialize(dummyCfg, dummyDict);
      }

      _mcRunStatistics_ = std::make_unique<snemo::rc::run_statistics>();
      _mcRunStatistics_->set_logging(_logging_);
      _mcRunStatistics_->set_runs(*_runList_);
      _mcRunStatistics_->set_activity_model(*_mcActivityModel_);
      snemo::time::time_duration timeStep = snemo::time::invalid_duration();
      if (config_.has_key("run_statistics.time_step")) {
        std::string timeStepRepr = config_.fetch_string("run_statistics.time_step");
        DT_LOG_DEBUG(_logging_, "Decoding run statistics' time step from '" << timeStepRepr << "'...");
        timeStep = snemo::time::time_duration_from_string(timeStepRepr);
      }
      if (not snemo::time::is_valid(timeStep)) {
        DT_LOG_DEBUG(_logging_, "Falling back to run statistics' default time step...");
        timeStep = snemo::time::minutes(15);
      }
      DT_LOG_DEBUG(_logging_, "Run statistics' time step is : " << time::to_string(timeStep));
      _mcRunStatistics_->set_time_step(timeStep);
      _mcRunStatistics_->initialize();
      if (datatools::logger::is_debug(_logging_)) {
        boost::property_tree::ptree popts;
        popts.put("title", "Run statistics: ");
        popts.put("indent", "[debug] ");
        _mcRunStatistics_->print_tree(std::cerr, popts);
      }

      long randomSeed = 0;
      std::uint32_t nbEvents = 0;
      if (config_.has_key("number_of_events")) {
        nbEvents = config_.fetch_positive_integer("number_of_events");
      }
      _mcEventDistribution_
        = std::make_unique<snemo::rc::mc_event_distribution>(*_mcRunStatistics_,
                                                             nbEvents,
                                                             randomSeed);
      if (datatools::logger::is_debug(_logging_)) {
        boost::property_tree::ptree popts;
        popts.put("title", "MC event distribution: ");
        popts.put("indent", "[debug] ");
        _mcEventDistribution_->print_tree(std::cerr, popts);
      }
      
      _initialized_ = true;
      return;
    }
     
    void event_timestamper::reset()
    {
      DT_THROW_IF(not is_initialized(), std::logic_error,
                  "Event timestamper is not initialized!");
      _mcEventDistribution_.reset();
      _mcRunStatistics_.reset();
      _mcActivityModel_.reset();
      if (_actModelFactory_.is_initialized()) {
        _actModelFactory_.reset();
      }
      _runList_.reset();
      _initialized_ = false;
      return;
    }

    void event_timestamper::process(snemo::datamodel::event_header & eh_) const
    {
      if (eh_.get_generation() != snemo::datamodel::event_header::GENERATION_SIMULATED) {
        DT_THROW(std::logic_error, "Event record is not a simulated event!");
      }
      DT_THROW_IF(not _mcEventDistribution_->has_next_decay(),
                  std::logic_error,
                  "The MC event distribution has no more event timestamps!");
      auto evenRcTiming = _mcEventDistribution_->next_decay();
      eh_.set_mc_run_id(evenRcTiming.run_id);
      eh_.set_mc_timestamp(evenRcTiming.decay_time);
      DT_LOG_DEBUG(_logging_, "Decay #" << _mcEventDistribution_->get_current_decay_index());
      DT_LOG_DEBUG(_logging_, "  Assign run ID = " << evenRcTiming.run_id);
      DT_LOG_DEBUG(_logging_, "  Assign decay timestamp = " << time::to_string(evenRcTiming.decay_time));
      return;
    }
        
  }  // end of namespace simulation

}  // end of namespace snemo

