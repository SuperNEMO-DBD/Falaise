// falaise/snemo/simulation/from_ascii_files_event_generator.cc

// Ourselves
#include <falaise/snemo/simulation/from_ascii_files_event_generator.h>

// Standard library:
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/multi_properties.h>
// - Bayeux/mygsl:
// - Bayeux/genbb_help:
#include <genbb_help/primary_event.h>
#include <genbb_help/pdg_particle_tools.h>
#include <bxdecay0/event.h>
#include <bxdecay0/event_reader.h>

namespace snemo {

  namespace simulation {

    struct from_ascii_files_event_generator::pimpl_type
    {
      bxdecay0::event_reader::config_type reader_config;
      bxdecay0::event_reader reader;
    };

    GENBB_PG_REGISTRATION_IMPLEMENT(from_ascii_files_event_generator,
				    "snemo::simulation::from_ascii_files_event_generator")

    bool from_ascii_files_event_generator::can_external_random() const
    {
      return false;
    }
    
    bool from_ascii_files_event_generator::is_initialized() const
    {
      return _initialized_;
    }

    from_ascii_files_event_generator::from_ascii_files_event_generator()
    {
      _initialized_ = false;
      _pimpl_ = std::make_unique<pimpl_type>();
      _at_reset_();
      return;
    }

    from_ascii_files_event_generator::~from_ascii_files_event_generator()
    {
      if (_initialized_) {
	reset();
      }
      _pimpl_.reset();
      return;
    }

    void from_ascii_files_event_generator::reset()
    {
      if (!_initialized_) {
	return;
      }
      _initialized_ = false;
      _at_reset_();
      return;
    }

    void from_ascii_files_event_generator::initialize(const datatools::properties & dps_,
						      datatools::service_manager & /*srv_mgr_*/,
						      genbb::detail::pg_dict_type & /*pg_dict_*/)
    {
      DT_THROW_IF(_initialized_, std::logic_error,
		  "Operation prohibited ! Object is already initialized !");

      _initialize_base(dps_);
      
      if (dps_.has_key("event_files")) {
	std::vector<std::string> eventFiles;
	dps_.fetch("event_files", eventFiles);
	for (auto eventFile : eventFiles) {
	  datatools::fetch_path_with_env(eventFile);
	  _pimpl_->reader_config.event_files.push_back(eventFile);
	}
      }
    
      if (dps_.has_key("start_event")) {
	_pimpl_->reader_config.start_event = dps_.fetch_positive_integer("start_event");
      }
    
      if (dps_.has_key("max_nb_events")) {
	_pimpl_->reader_config.max_nb_events = dps_.fetch_positive_integer("max_nb_events");
      }
    
      if (dps_.has_key("zero_event_time")) {
    	_pimpl_->reader_config.zero_event_time = dps_.fetch_boolean("zero_event_time");
      }
  
      _at_init_();

      _initialized_ = true;
      return;
    }

    void from_ascii_files_event_generator::_at_init_()
    {
      _pimpl_->reader.set_configuration(_pimpl_->reader_config);
      return;
    }

    void from_ascii_files_event_generator::_at_reset_()
    {
      if (_pimpl_->reader.is_configured()) {
	_pimpl_->reader.reset_configuration();
      }
      return;
    }

    bool from_ascii_files_event_generator::has_next()
    {
      return _pimpl_->reader.is_configured() && _pimpl_->reader.has_next_event();
    }

    void from_ascii_files_event_generator::_load_next(::genbb::primary_event & event_,
						      bool compute_classification_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");
      DT_THROW_IF(!_initialized_, std::logic_error, "Generator is not locked/initialized !");
      event_.reset();

      bxdecay0::event ev;
      _pimpl_->reader.load_next_event(ev);

      // Fill the Bayeux/Genbb primary event from the BxDecay0 event...
      if (ev.has_generator()) {
	event_.set_label(ev.get_generator());
      }
      event_.set_time(0.0);
      if (ev.has_time()) {
	event_.set_time(ev.get_time() * CLHEP::second);
      }
      const std::vector<bxdecay0::particle> & evParticles = ev.get_particles();
      for (const auto & evParticle : evParticles) {
	::genbb::primary_particle & newPart = event_.add_particle();
	if (evParticle.is_electron()) {
	  newPart.set_type(::genbb::primary_particle::particle_type::ELECTRON);
	  newPart.set_pdg_code(::genbb::pdg::particle::ELECTRON);
	} else if (evParticle.is_positron()) {
	  newPart.set_type(::genbb::primary_particle::particle_type::POSITRON);
	  newPart.set_pdg_code(::genbb::pdg::particle::POSITRON);
	} else if (evParticle.is_gamma()) {
	  newPart.set_type(::genbb::primary_particle::particle_type::GAMMA);
	  newPart.set_pdg_code(::genbb::pdg::particle::GAMMA);
	} else if (evParticle.is_alpha()) {
	  newPart.set_type(::genbb::primary_particle::particle_type::ALPHA);
	  newPart.set_pdg_code(::genbb::pdg::particle::ALPHA);
	}
	if (evParticle.has_time()) {
	  newPart.set_time(evParticle.get_time() * CLHEP::second);
	}
	double px = evParticle.get_px();
	double py = evParticle.get_py();
	double pz = evParticle.get_pz();
	newPart.grab_momentum().setX(px * CLHEP::MeV);
	newPart.grab_momentum().setY(py * CLHEP::MeV);
	newPart.grab_momentum().setZ(pz * CLHEP::MeV);
      }
      
      if (compute_classification_) {
	event_.compute_classification();
      }
      
      DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
      return;
    }

  } // end of namespace simulation

} // end of namespace snemo
