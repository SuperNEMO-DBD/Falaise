// -*- mode: c++ ; -*-
/** \file  particle_track_data.cc */

// Ourselves
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
           << "Particles : " << _particles_.size () << std::endl;

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

// end of falaise/snemo/datamodels/particle_track_data.cc
