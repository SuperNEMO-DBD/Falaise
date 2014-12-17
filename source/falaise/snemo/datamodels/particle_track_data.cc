// particle_track_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/particle_track_data.h>

namespace snemo {

  namespace datamodel {

    bool particle_track_data::has_particles() const
    {
      return get_number_of_particles () > 0;
    }

    size_t particle_track_data::get_number_of_particles() const
    {
      return _particles_.size ();
    }

    void particle_track_data::add_particle(const particle_track::handle_type & particle_handle_)
    {
      for (size_t i = 0; i < _particles_.size (); i++)
        {
          particle_track::handle_type h = _particles_.at (i);
          particle_track * addr = &(h.grab());
          DT_THROW_IF (addr == &(particle_handle_.get()), std::logic_error,
                       "Duplicated particles is not allowed!");
        }
      _particles_.push_back(particle_handle_);
      return;
    }

    const particle_track & particle_track_data::get_particle(size_t i_) const
    {
      return _particles_.at(i_).get();
    }

    particle_track_data::particle_collection_type & particle_track_data::grab_particles()
    {
      return _particles_;
    }

    const particle_track_data::particle_collection_type & particle_track_data::get_particles() const
    {
      return _particles_;
    }

    void particle_track_data::invalidate_particles()
    {
      _particles_.clear();
      return;
    }

    size_t particle_track_data::extract_particles(particle_collection_type & particles_,
                                                  const particle_track::charge_type charge_,
                                                  const bool clear_) const
    {
      if (clear_) particles_.clear();
      size_t ipart = 0;
      for (particle_collection_type::const_iterator i = get_particles().begin();
           i != get_particles().end(); ++i) {
        const particle_track::handle_type & a_particle = *i;
        const particle_track::charge_type a_charge = a_particle.get().get_charge();
        if (a_charge == charge_) {
          particles_.push_back(a_particle);
          ipart++;
        }
      }
      return ipart;
    }

    bool particle_track_data::has_non_associated_calorimeters() const
    {
      return get_non_associated_calorimeters().size() > 0;
    }

    calibrated_calorimeter_hit::collection_type & particle_track_data::grab_non_associated_calorimeters()
    {
      return _non_associated_calorimeters_;
    }

    const calibrated_calorimeter_hit::collection_type & particle_track_data::get_non_associated_calorimeters() const
    {
      return _non_associated_calorimeters_;
    }

    void particle_track_data::reset_non_associated_calorimeters()
    {
      _non_associated_calorimeters_.clear();
      return;
    }

    datatools::properties & particle_track_data::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    const datatools::properties & particle_track_data::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    void particle_track_data::reset()
    {
      this->clear();
      return;
    }

    void particle_track_data::clear()
    {
      invalidate_particles();
      reset_non_associated_calorimeters();
      _auxiliaries_.clear();
      return;
    }

    particle_track_data::particle_track_data()
    {
      return;
    }

    particle_track_data::~particle_track_data()
    {
      this->reset ();
      return;
    }

    void particle_track_data::tree_dump (std::ostream      & out_,
                                         const std::string & title_,
                                         const std::string & indent_,
                                         bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty ())
        {
          indent = indent_;
        }
      if ( ! title_.empty () )
        {
          out_ << indent << title_ << std::endl;
        }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Particle(s) : " << _particles_.size () << std::endl;

      for (size_t i = 0; i < get_number_of_particles(); i++) {
        const particle_track & ptrack = get_particle(i);
        std::ostringstream indent2;
        out_ << indent << datatools::i_tree_dumpable::skip_tag;
        indent2 << indent << datatools::i_tree_dumpable::skip_tag;
        if (i == get_number_of_particles() - 1) {
          out_ << datatools::i_tree_dumpable::last_tag;
          indent2 << datatools::i_tree_dumpable::last_skip_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent2 << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Particle #" << i << " : " << std::endl;
        ptrack.tree_dump(out_, "", indent2.str());
      }

      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Auxiliaries : ";
      if (_auxiliaries_.empty ())
        {
          out_ << "<empty>";
        }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_) ;
        _auxiliaries_.tree_dump (out_, "", indent_oss.str ());
      }

      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(particle_track_data, "snemo::datamodel::particle_track_data")

  } // end of namespace model

} // end of namespace snemo
