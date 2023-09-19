// falaise/snemo/simulation/arbitrary_event_generator_injector.cc

// Ourselves
#include <falaise/snemo/simulation/arbitrary_event_generator_injector.h>

// Standard library:
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
// #include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/multi_properties.h>
// - Bayeux/mygsl:
// #include <mygsl/histogram.h>
// #include <mygsl/i_unary_function.h>
// #include <mygsl/tabulated_function.h>
// #include <mygsl/von_neumann_method.h>
// - Bayeux/genbb_help:
#include <genbb_help/primary_event.h>
// #include <genbb_help/single_particle_generator.h>

namespace snemo {

  namespace simulation {

    GENBB_PG_REGISTRATION_IMPLEMENT(arbitrary_event_generator_injector, "snemo::simulation::arbitrary_event_generator_injector")

    bool arbitrary_event_generator_injector::can_external_random() const
    {
      return true;
    }
    
    bool arbitrary_event_generator_injector::is_initialized() const
    {
      return _initialized_;
    }

    arbitrary_event_generator_injector::arbitrary_event_generator_injector()
    {
      _initialized_ = false;

      _at_reset_();

      _seed_ = 0;
    }

    arbitrary_event_generator_injector::~arbitrary_event_generator_injector()
    {
      if (_initialized_) {
	reset();
      }
    }

    const mygsl::rng & arbitrary_event_generator_injector::get_random() const
    {
      if (has_external_random()) {
	return get_external_random();
      }
      return _random_;
    }

    mygsl::rng & arbitrary_event_generator_injector::grab_random()
    {
      if (has_external_random()) {
	return grab_external_random();
      }
      return _random_;
    }

    void arbitrary_event_generator_injector::reset()
    {
      if (!_initialized_) {
	return;
      }
      _initialized_ = false;
      _at_reset_();
      return;
    }

    void arbitrary_event_generator_injector::initialize(const datatools::properties & dps_,
							datatools::service_manager & srv_mgr_,
							genbb::detail::pg_dict_type & pg_dict_)
    {
      DT_THROW_IF(_initialized_, std::logic_error,
		  "Operation prohibited ! Object is already initialized !");

      _initialize_base(dps_);

      // Link to the current dictionary of already setup generators:
      _mgr_.set_service_manager(srv_mgr_);
      _mgr_.set_logging_priority(get_logging_priority());
      _pg_dict_ = &pg_dict_;
      
      if (!has_external_random()) {
	DT_THROW_IF(!dps_.has_key("seed"), std::logic_error,
		    "Missing 'seed' property for particle generator '" << get_name() << "' !");
	long seed = dps_.fetch_integer("seed");
	DT_THROW_IF(seed < 0, std::logic_error,
		    "Invalid seed value (>=0) for particle generator '" << get_name() << "' !");
	_seed_ = seed;
      }
      
      if (dps_.has_key("list_of_setup_files")) {
	std::string list_of_setup_files_paths = dps_.fetch_string("list_of_setup_files");
	datatools::fetch_path_with_env(list_of_setup_files_paths);
	std::ifstream setupListFile(list_of_setup_files_paths);
	while (!setupListFile.eof()) {
	  std::string line;
	  std::getline(setupListFile, line);
	  if (!line.empty() and line[0] != '#') {
	    std::string setupFilePath = line;
	    _setup_file_paths_.push_back(setupFilePath);
	  }
	}
      }
      
      if (dps_.has_key("setup_file")) {
	std::string setupFilePath = dps_.fetch_string("setup_file");
	_setup_file_paths_.push_back(setupFilePath);
      }
    
      if (dps_.has_key("generator")) {
	std::string generator_name = dps_.fetch_string("generator");
	_pg_name_ = generator_name;
      }
      if (_pg_name_.empty()) {
	// Default generator name
	_pg_name_ = "aegir";
      }

      DT_THROW_IF(_setup_file_paths_.size() == 0, std::logic_error,
		  "Missing 'setup_file' or 'list_of_setup_files' property for particle generator '" << get_name() << "' !"); 

      _at_init_();

      _initialized_ = true;
      return;
    }

    void arbitrary_event_generator_injector::_at_init_()
    {
      // Initialize the PRNG :
      if (has_external_random()) {
	_mgr_.set_external_prng(grab_external_random());
      } 
      if (!has_external_random()) {
	_random_.init("taus2", _seed_);
	_mgr_.set_external_prng(_random_);
      }
      for (std::string setupFile : _setup_file_paths_) {
	datatools::fetch_path_with_env(setupFile);
	datatools::multi_properties managerConfig;
	managerConfig.read(setupFile,
			   datatools::multi_properties::config::SKIP_PRIVATE_SECTIONS);
	_mgr_.load(managerConfig);
      }

      DT_THROW_IF(! _mgr_.has(_pg_name_), std::logic_error,
		  "No '" << _pg_name_ << "' particle generator is set!");
      _pg_ = & _mgr_.grab(_pg_name_);
      
      return;
    }

    void arbitrary_event_generator_injector::_at_reset_()
    {
      _pg_ = nullptr;
      if (_mgr_.is_initialized()) {
	_mgr_.reset();
      }
      _setup_file_paths_.clear();
      if (_random_.is_initialized()) {
	_random_.reset();
      }
      _seed_ = 0;
      return;
    }

    bool arbitrary_event_generator_injector::has_next()
    {
      return _pg_->has_next();
    }

    void arbitrary_event_generator_injector::_load_next(::genbb::primary_event & event_,
							bool compute_classification_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");
      DT_THROW_IF(!_initialized_, std::logic_error, "Generator is not locked/initialized !");
      event_.reset();
      _pg_->load_next(event_, compute_classification_);
      if (compute_classification_) {
	event_.compute_classification();
      }
      DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
      return;
    }

  } // end of namespace simulation

} // end of namespace snemo
