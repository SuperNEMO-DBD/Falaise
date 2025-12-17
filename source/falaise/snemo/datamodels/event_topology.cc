// falaise/snemo/datamodels/event_topology.cc

// Ourselves:
#include <falaise/snemo/datamodels/event_topology.h>

namespace snemo {

  namespace datamodel {

    void event_topology::set_origin_timestamp(const timestamp & origin_timestamp_)
    {
      _origin_timestamp_ = origin_timestamp_;
      return;
    }

    const timestamp & event_topology::get_origin_timestamp() const
    {
      return _origin_timestamp_;
    }

    bool event_topology::has_origin_vertex() const
    {
      return _origin_vertex_handle_.has_data();
    }

    const VertexHdl & event_topology::get_origin_vertex_handle() const
    {
      return _origin_vertex_handle_;
    }

    const Vertex & event_topology::get_origin_vertex() const
    {
      DT_THROW_IF(not has_origin_vertex(), std::logic_error,
		  "No origin vertex is set");
      return *_origin_vertex_handle_;
    }

    void event_topology::set_origin_vertex(const VertexHdl & origin_vertex_handle_)
    {
      _origin_vertex_handle_ = origin_vertex_handle_;
      return;
    }

    std::size_t event_topology::number_of_particles() const
    {
      return _particles_.size();
    }

    void event_topology::add_particle(const ReconstructedParticleHdl & particle_handle_)
    {
      DT_THROW_IF(not particle_handle_.has_data(), std::logic_error,
		  "Missing handled data");
      _particles_.push_back(particle_handle_);
      return;
    }

    const ReconstructedParticleHdl &
    event_topology::get_particle_handle(const std::uint32_t index_) const
    {
      DT_THROW_IF(index_ >= _particles_.size(),
		  std::range_error,
		  "Invalid particle index for event topology likelihood");	       
      return _particles_[index_];
    }

    const ReconstructedParticle &
    event_topology::get_particle(const std::uint32_t index_) const
    {
      DT_THROW_IF(index_ >= _particles_.size(),
		  std::range_error,
		  "Invalid particle index for event topology likelihood");	       
      return *(_particles_[index_]);
    }

    void event_topology::set_likelihood(const double likelihood_)
    {
      DT_THROW_IF(likelihood_ < 0.0 or likelihood_ >= 1.0,
		  std::range_error,
		  "Invalid value for event topology likelihood");	       
      _likelihood_ = likelihood_;
      return;
    }

    double event_topology::get_likelihood() const
    {
      return _likelihood_;
    }
	
    void event_topology::print_tree(std::ostream & out_,
				    const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_particles_opt = options_.get("list_particles", true);
      const std::string & indent = popts.indent;

      out_ << indent << tag << "Origin timestamp : " << _origin_timestamp_.to_string() << std::endl;

      out_ << indent << tag << "Origin vertex : ";
      if (not has_origin_vertex()) {
	out_ << "none";
      }
      out_ << std::endl;
      if (has_origin_vertex()) {
	std::string indentItem = indent;
	indentItem += tags::skip_item();
	boost::property_tree::ptree vtxOpts;
	vtxOpts.put("indent", indentItem); 
	_origin_vertex_handle_->print_tree(out_, vtxOpts);
      }

      out_ << indent << tag << "Particles : ";
      if (_particles_.size()) {
	out_ << _particles_.size();
      } else {
	out_ << "none";
      }
      out_ << std::endl;
      if (list_particles_opt and _particles_.size()) {
	auto count = 0u;
	for (auto i = 0u; i < _particles_.size(); i++) {
	  std::string tag = tags::item();
	  std::string skip_tag = tags::skip_item();
	  if (i + 1 == _particles_.size()) {
	    tag = tags::last_item();
	    skip_tag = tags::last_skip_item();
	  }
	  out_ << indent << tags::skip_item() << tag << "Particle #" << i << " : " << std::endl;
	  const auto & partHdl = _particles_[i];
	  {
	    boost::property_tree::ptree poptions;
	    poptions.put("indent", indent + tags::skip_item() + skip_tag);
	    partHdl->print_tree(out_, poptions);
	  }
	  count++;
	}
       }
 
      out_ << indent << inherit_tag(popts.inherit)
	  << "Likelihood : " << _likelihood_ << std::endl;
             
      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(event_topology,
                                                      "snemo::datamodel::event_topology")

  } // end of namespace datamodel

} // end of namespace snemo
