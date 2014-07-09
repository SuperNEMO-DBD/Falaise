// falaise/snemo/datamodels/particle_track.cc

// Ourselves:
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

  namespace datamodel {

    const std::string & particle_track::vertex_type_key()
    {
      static const std::string _flag("vertex.type");
      return _flag;
    }

    const std::string & particle_track::vertex_type_to_label(vertex_type vt_)
    {
      switch(vt_) {
      case VERTEX_ON_SOURCE_FOIL: return vertex_on_source_foil_label();
      case VERTEX_ON_MAIN_CALORIMETER: return vertex_on_main_calorimeter_label();
      case VERTEX_ON_X_CALORIMETER: return vertex_on_x_calorimeter_label();
      case VERTEX_ON_GAMMA_VETO: return vertex_on_gamma_veto_label();
      case VERTEX_ON_WIRE: return vertex_on_wire_label();
      default:
        return vertex_none_label();
      }
    }

    particle_track::vertex_type particle_track::label_to_vertex_type(const std::string & label_)
    {
      if (label_ == vertex_on_source_foil_label()) return VERTEX_ON_SOURCE_FOIL;
      if (label_ == vertex_on_main_calorimeter_label()) return VERTEX_ON_MAIN_CALORIMETER;
      if (label_ == vertex_on_x_calorimeter_label()) return VERTEX_ON_X_CALORIMETER;
      if (label_ == vertex_on_gamma_veto_label()) return VERTEX_ON_GAMMA_VETO;
      if (label_ == vertex_on_wire_label()) return VERTEX_ON_WIRE;
      return VERTEX_NONE;
    }

    const std::string & particle_track::vertex_none_label()
    {
      static const std::string _flag("!");
      return _flag;
    }

    const std::string & particle_track::vertex_on_wire_label()
    {
      static const std::string _flag("wire");
      return _flag;
    }

    const std::string & particle_track::vertex_on_source_foil_label()
    {
      static const std::string _flag("foil");
      return _flag;
    }

    const std::string & particle_track::vertex_on_main_calorimeter_label()
    {
      static const std::string _flag("calo");
      return _flag;
    }

    const std::string & particle_track::vertex_on_x_calorimeter_label()
    {
      static const std::string _flag("xcalo");
      return _flag;
    }

    const std::string & particle_track::vertex_on_gamma_veto_label()
    {
      static const std::string _flag("gveto");
      return _flag;
    }

    bool particle_track::vertex_is(const geomtools::blur_spot & vtx_, vertex_type vtype_)
    {
      if (vtx_.get_auxiliaries().has_key(vertex_type_key())) {
        std::string vtype_label = vtx_.get_auxiliaries().fetch_string(vertex_type_key());
        return vtype_label == vertex_type_to_label(vtype_);
      } else {
        if (vtype_ == VERTEX_NONE) {
          return true;
        }
      }
      return false;
    }

    bool particle_track::vertex_is_on_source_foil(const geomtools::blur_spot & vtx_)
    {
      return vertex_is(vtx_, VERTEX_ON_SOURCE_FOIL);
    }

    bool particle_track::vertex_is_on_main_calorimeter(const geomtools::blur_spot & vtx_)
    {
      return vertex_is(vtx_, VERTEX_ON_MAIN_CALORIMETER);
    }

    bool particle_track::vertex_is_on_x_calorimeter(const geomtools::blur_spot & vtx_)
    {
      return vertex_is(vtx_, VERTEX_ON_X_CALORIMETER);
    }

    bool particle_track::vertex_is_on_gamma_veto(const geomtools::blur_spot & vtx_)
    {
      return vertex_is(vtx_, VERTEX_ON_GAMMA_VETO);
    }

    bool particle_track::vertex_is_on_wire(const geomtools::blur_spot & vtx_)
    {
      return vertex_is(vtx_, VERTEX_ON_WIRE);
    }

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

    size_t particle_track::compute_vertices(vertex_collection_type & vertices_, const uint32_t flags_, const bool clear_) const
    {
      if (clear_) vertices_.clear();
      size_t ivtx = 0;
      for (vertex_collection_type::const_iterator i = get_vertices().begin();
           i != get_vertices().end(); ++i)
        {
          const handle_spot & a_vertex = *i;
          const datatools::properties & aux = a_vertex.get().get_auxiliaries();
          std::string vtx_type;
          if (aux.has_key(vertex_type_key())) {
            vtx_type = aux.fetch_string(vertex_type_key());
          }
          const bool has_vertex_on_foil  =
            (flags_ & VERTEX_ON_SOURCE_FOIL)
            && vertex_is_on_source_foil(a_vertex.get());
          const bool has_vertex_on_calo  =
            (flags_ & VERTEX_ON_MAIN_CALORIMETER)
            && vertex_is_on_main_calorimeter(a_vertex.get());
          const bool has_vertex_on_xcalo =
            (flags_ & VERTEX_ON_X_CALORIMETER)
            && vertex_is_on_x_calorimeter(a_vertex.get());
          const bool has_vertex_on_gveto =
            (flags_ & VERTEX_ON_GAMMA_VETO)
            && vertex_is_on_gamma_veto(a_vertex.get());
          const bool has_vertex_on_wire  =
            (flags_ & VERTEX_ON_WIRE)
            && vertex_is_on_wire(a_vertex.get());

          if (has_vertex_on_foil  ||
              has_vertex_on_calo  ||
              has_vertex_on_xcalo ||
              has_vertex_on_gveto ||
              has_vertex_on_wire) {
            vertices_.push_back(a_vertex);
            ivtx++;
          }
        }
      return ivtx;
    }

    bool particle_track::has_associated_calorimeter_hits() const
    {
      return !_associated_calorimeter_hits_.empty();
    }

    void particle_track::reset_associated_calorimeter_hits()
    {
      _associated_calorimeter_hits_.clear();
      return;
    }

    calibrated_calorimeter_hit::collection_type & particle_track::grab_associated_calorimeter_hits()
    {
      return _associated_calorimeter_hits_;
    }

    const calibrated_calorimeter_hit::collection_type & particle_track::get_associated_calorimeter_hits() const
    {
      return _associated_calorimeter_hits_;
    }

    void particle_track::reset()
    {
      this->clear();
      return;
    }

    void particle_track::clear()
    {
      reset_vertices();
      reset_associated_calorimeter_hits();
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
      if (has_vertices())
        {
          out_ << _vertices_.size();
        }
      else
        {
          out_ << "<No>";
        }
      out_ << std::endl;
      for (vertex_collection_type::const_iterator i = _vertices_.begin();
           i != _vertices_.end();
           i++) {
        out_ << indent << datatools::i_tree_dumpable::skip_tag;
        vertex_collection_type::const_iterator j = i;
        j++;
        if (j == _vertices_.end()) {
          out_ << datatools::i_tree_dumpable::last_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
        }
        const geomtools::blur_spot & spot = i->get();
        out_ << "Vertex Id=" << spot.get_hit_id()
             << " @ " << spot.get_placement().get_translation() / CLHEP::mm << " mm"
             << " (" << spot.get_auxiliaries().fetch_string(vertex_type_key()) << ")";

        out_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Associated calorimeter hit(s) : ";
      if (has_associated_calorimeter_hits())
        {
          out_ << _associated_calorimeter_hits_.size ();
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
