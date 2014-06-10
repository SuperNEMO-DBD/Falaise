// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/particle_track.cc */

// Ourselves
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

  namespace datamodel {

    bool particle_track::has_track_id() const
    {
      return has_hit_id();
    }

    int particle_track::get_track_id() const
    {
      return get_hit_id();
    }

    void particle_track::set_track_id(int32_t track_id_)
    {
      set_hit_id(track_id_);
      return;
    }

    void particle_track::invalidate_track_id()
    {
      invalidate_hit_id();
      return;
    }

    void particle_track::set_charge(int8_t charge_)
    {
      _charge_from_source_ = charge_;
      return;
    }

    int8_t particle_track::get_charge() const
    {
      return _charge_from_source_;
    }

    bool particle_track::has_negative_charge() const
    {
      if (_charge_from_source_ == negative) return true;
      return false;
    }

    particle_track::particle_track()
    {
      _charge_from_source_ = invalid;
      invalidate_track_id();
      return;
    }

    particle_track::~particle_track()
    {
      this->reset();
      return;
    }

    bool particle_track::has_trajectory() const
    {
      return _trajectory_.has_data();
    }

    void particle_track::detach_trajectory()
    {
      _trajectory_.reset();
      return;
    }

    tracker_trajectory::handle_type & particle_track::grab_trajectory_handle()
    {
      return _trajectory_;
    }

    const tracker_trajectory::handle_type & particle_track::get_trajectory_handle () const
    {
      return _trajectory_;
    }

    void particle_track::set_trajectory_handle(const tracker_trajectory::handle_type & trajectory_handle_)
    {
      _trajectory_ = trajectory_handle_;
      return;
    }

    tracker_trajectory & particle_track::grab_trajectory()
    {
      return _trajectory_.grab();
    }

    const tracker_trajectory & particle_track::get_trajectory() const
    {
      return _trajectory_.get();
    }

    bool particle_track::has_vertices() const
    {
      return _vertices_.size() > 0;
    }

    void particle_track::reset_vertices()
    {
      _vertices_.clear();
      return;
    }

    particle_track::vertex_collection_type & particle_track::grab_vertices()
    {
      return _vertices_;
    }

    const particle_track::vertex_collection_type & particle_track::get_vertices () const
    {
      return _vertices_;
    }

    bool particle_track::has_associated_calorimeters() const
    {
      return !_associated_calorimeters_.empty();
    }

    void particle_track::reset_associated_calorimeters()
    {
      _associated_calorimeters_.clear();
      return;
    }

    calibrated_calorimeter_hit::collection_type & particle_track::grab_associated_calorimeters()
    {
      return _associated_calorimeters_;
    }

    const calibrated_calorimeter_hit::collection_type & particle_track::get_associated_calorimeters() const
    {
      return _associated_calorimeters_;
    }

    void particle_track::reset()
    {
      this->clear();
      return;
    }

    void particle_track::clear()
    {
      reset_vertices();
      reset_associated_calorimeters();
      detach_trajectory();
      base_hit::clear();

      invalidate_track_id();
      _charge_from_source_ = invalid;
      return;
    }

    void particle_track::tree_dump(std::ostream      & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty ())
        {
          indent = indent_;
        }
      base_hit::tree_dump (out_, title_, indent, true);

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Track ID : " << get_track_id () << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Trajectory : ";
      if (has_trajectory ())
        {
          out_ << _trajectory_.get ().get_trajectory_id () ;
        }
      else
        {
          out_ << "<No>";
        }
      out_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Particle charge : ";
      if (get_charge () == invalid)       out_ << "invalid";
      else if (get_charge () == negative) out_ << "negative";
      else if (get_charge () == positive) out_ << "positive";
      out_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Vertices : ";
      if (has_vertices ())
        {
          out_ << _vertices_.size ();
        }
      else
        {
          out_ << "<No>";
        }
      out_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Associated calorimeter(s) : ";
      if (has_associated_calorimeters ())
        {
          out_ << _associated_calorimeters_.size ();
        }
      else
        {
          out_ << "<No>";
        }
      out_ << std::endl;

      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(particle_track, "snemo::datamodel::particle_track")

  } // end of namespace datamodel

} // end of namespace snemo

// end of falaise/snemo/datamodels/particle_track.cc
