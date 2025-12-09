// falaise/snemo/datamodels/reconstructed_particle.cc

// Ourselves:
#include <falaise/snemo/datamodels/reconstructed_particle.h>

namespace snemo {

  namespace datamodel {

    bool reconstructed_particle::is_gamma() const
    {
      return _particle_code_ == genbb::pdg::particle::GAMMA;
    } 

    void reconstructed_particle::set_gamma()
    {
      _particle_code_ = genbb::pdg::particle::GAMMA;
      return;
    }

    bool reconstructed_particle::is_electron() const
    {
      return _particle_code_ == genbb::pdg::particle::ELECTRON;
    }

    void reconstructed_particle::set_electron()
    {
      _particle_code_ = genbb::pdg::particle::ELECTRON;
      return;
    }

    bool reconstructed_particle::is_positron() const
    {
      return _particle_code_ == genbb::pdg::particle::POSITRON;
    }

    void reconstructed_particle::set_positron()
    {
      _particle_code_ = genbb::pdg::particle::POSITRON;
      return;
    }

    bool reconstructed_particle::is_alpha() const
    {
      return _particle_code_ == genbb::pdg::particle::ALPHA;
    }

    void reconstructed_particle::set_alpha()
    {
      _particle_code_ = genbb::pdg::particle::ALPHA;
      return;
    }

    bool reconstructed_particle::is_muon() const
    {
      return _particle_code_ == genbb::pdg::particle::MUON_MINUS
	or _particle_code_ == genbb::pdg::particle::MUON_PLUS;
    }

    void reconstructed_particle::set_muon_minus()
    {
      _particle_code_ = genbb::pdg::particle::MUON_MINUS;
      return;
    }

    void reconstructed_particle::set_muon_plus()
    {
      _particle_code_ = genbb::pdg::particle::MUON_PLUS;
      return;
    }

    particle_code reconstructed_particle::get_particle_code() const
    {
      return _particle_code_;
    }

    void reconstructed_particle::set_particle_code(const particle_code particle_code_)
    {
      _particle_code_ = particle_code_;
      return;
    }

    void reconstructed_particle::set_prompt()
    {
      _delayed_ = false;
      return;
    }

    void reconstructed_particle::set_delayed()
    {
      _delayed_ = true;
      return;
    }
    
    bool reconstructed_particle::is_delayed() const
    {
      return _delayed_;
    }

    bool reconstructed_particle::is_prompt() const
    {
     return not is_delayed();
    }

    /*
    const ParticleHdl & reconstructed_particle::get_track_handle() const
    {
      return _track_handle_;
    }

    const Particle & reconstructed_particle::get_track() const
    {
      return *_track_handle_;
    }
    
    void reconstructed_particle::set_track_handle(const ParticleHdl & track_handle_)
    {
      DT_THROW_IF(not track_handle_.has_data(), std::logic_error,
		  "Missing handle track data");
     _track_handle_ = track_handle_;
      return;
    }
    */
    
    void reconstructed_particle::print_tree(std::ostream & out_,
					    const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
       const std::string & indent = popts.indent;

      out_ << indent << tag << "Particle: "
	   << genbb::pdg::particle_registry::const_system_registry().get_particle(_particle_code_).get_name()
	   << " [code=" << _particle_code_ << ']'
	   << std::endl;

      out_ << indent << tag << "Delayed: " << std::boolalpha << _delayed_ << std::endl;

      out_ << indent << tag << "Start timestamp: " << _start_timestamp_ << std::endl;

      out_ << indent << tag << "Stop timestamp: " << _stop_timestamp_ << std::endl;

      out_ << indent << inherit_tag(popts.inherit) << "Track: ";
      if (_track_handle_.has_data()) {
	out_ << _track_handle_->get_track_id();
      } else {
	out_ << "none";
      }
      out_ << std::endl;

  
      return;
    }
    
    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(reconstructed_particle,
                                                      "snemo::datamodel::reconstructed_particle")

  } // end of namespace datamodel

} // end of namespace snemo
