// falaise/snemo/datamodels/particle_track.cc

// Ourselves:
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

  namespace datamodel {

    // static
    particle_track::charge_type particle_track::string_to_charge(const std::string & label_)
    {
      if (label_ == to_string(particle_track::CHARGE_NEUTRAL)) return particle_track::CHARGE_NEUTRAL;
      if (label_ == to_string(particle_track::CHARGE_POSITIVE)) return particle_track::CHARGE_POSITIVE;
      if (label_ == to_string(particle_track::CHARGE_NEGATIVE)) return particle_track::CHARGE_NEGATIVE;
      return particle_track::CHARGE_UNDEFINED;
    }

    // static
    const std::string & particle_track::to_string(const particle_track::charge_type charge_)
    {
      static const std::string _neutral("neutral");
      static const std::string _positive("positive");
      static const std::string _negative("negative");
      static const std::string _undef("undefined");
      switch (charge_) {
      case particle_track::CHARGE_NEUTRAL:
        return _neutral;
      case particle_track::CHARGE_POSITIVE:
        return _positive;
      case particle_track::CHARGE_NEGATIVE:
        return _negative;
      default:
        break;
      }
      return _undef;
    }

    bool particle_has(const Particle & p, Particle::charge_type charge)
    {
      return p.get_charge() == charge;
    }

    bool particle_has_negative_charge(const Particle & p)
    {
      return particle_has(p, Particle::CHARGE_NEGATIVE);
    }

    bool particle_has_positive_charge(const Particle & p)
    {
      return particle_has(p, Particle::CHARGE_POSITIVE);
    }

    bool particle_has_undefined_charge(const Particle & p)
    {
      return particle_has(p, Particle::CHARGE_UNDEFINED);
    }

    bool particle_has_neutral_charge(const Particle & p)
    {
      return particle_has(p, Particle::CHARGE_NEUTRAL);
    }

    bool particle_has_vertex(const Particle & p_, const uint32_t vertex_flags_)
    {
      for (const auto & vtx : p_.get_vertices()) {
        if (vtx->match_categories(vertex_flags_)) {
          return true;
        }
      }
      return false;
    }

    /* Methods */
    
    bool particle_track::has_track_id() const
    {
      return has_hit_id();
    }

    int particle_track::get_track_id() const
    {
      return get_hit_id();
    }

    void particle_track::set_track_id(int32_t id)
    {
      set_hit_id(id);
      return;
    }

    void particle_track::set_charge(charge_type charge)
    {
      charge_from_source_ = charge;
      return;
    }

    particle_track::charge_type particle_track::get_charge() const
    {
      return charge_from_source_;
    }

    bool particle_track::has_trajectory() const
    {
      return trajectory_.has_data();
    }

    bool particle_track::match_cluster(const tracker_cluster & cluster_) const
    {
      if (!has_trajectory()) return false;
      if (!trajectory_->has_cluster()) return false;
      return &trajectory_->get_cluster() == &cluster_;
    }

    void particle_track::detach_trajectory()
    {
      trajectory_.reset();
      return;
    }

    TrackerTrajectoryHdl & particle_track::get_trajectory_handle()
    {
      return trajectory_;
    }

    const TrackerTrajectoryHdl & particle_track::get_trajectory_handle() const
    {
      return trajectory_;
    }

    void particle_track::set_trajectory_handle(const TrackerTrajectoryHdl & trajectory)
    {
      trajectory_ = trajectory;
      return;
    }

    tracker_trajectory & particle_track::get_trajectory()
    {
      return *trajectory_;
    }

    const tracker_trajectory & particle_track::get_trajectory() const
    {
      return *trajectory_;
    }

    bool particle_track::has_vertices() const
    {
      return !vertices_.empty();
    }

    void particle_track::clear_vertices()
    {
      vertices_.clear();
      return;
    }

    VertexHdlCollection & particle_track::get_vertices()
    {
      return vertices_;
    }

    const VertexHdlCollection & particle_track::get_vertices() const
    {
      return vertices_;
    }
   
    bool particle_track::has_associated_calorimeter_hits() const
    {
      return !associated_calorimeters_.empty();
    }

    void particle_track::clear_associated_calorimeter_hits()
    {
      associated_calorimeters_.clear();
      return;
    }

    CalorimeterHitHdlCollection & particle_track::get_associated_calorimeter_hits()
    {
      return associated_calorimeters_;
    }

    const CalorimeterHitHdlCollection & particle_track::get_associated_calorimeter_hits() const
    {
      return associated_calorimeters_;
    }

    CaloVertexAssociationCollection &
    particle_track::get_calo_vertex_associations()
    {
      return calo_vertex_associations_;
    }

    const CaloVertexAssociationCollection &
    particle_track::get_calo_vertex_associations() const
    {
      return calo_vertex_associations_;
    }

    void particle_track::clear()
    {
      calo_vertex_associations_.clear();
      clear_vertices();
      clear_associated_calorimeter_hits();
      detach_trajectory();
      charge_from_source_ = CHARGE_UNDEFINED;
      base_hit::clear();
      return;
    }

    void particle_track::print_tree(std::ostream & out,
                                    const boost::property_tree::ptree & options) const
    {
      base_hit::print_tree(out, base_print_options::force_inheritance(options));
      base_print_options popts;
      popts.configure_from(options);
      const std::string & indent = popts.indent;
 
      out << indent << tag << "Track ID : "
          << get_track_id() << std::endl;

      out << indent << tag << "Trajectory : ";
      if (has_trajectory()) {
        out << trajectory_->get_id();
      } else {
        out << "<none>";
      }
      out << std::endl;

      out << indent << tag << "Particle charge : '" << to_string(get_charge()) << "'" << std::endl;
 
      out << indent << tag << "Vertices[" << vertices_.size()
          << "] :" << std::endl;
      for (auto i = vertices_.begin(); i != vertices_.end(); i++) {
        out << indent << skip_tag;
        auto j = i;
        j++;
        std::string indentItem = indent;
        if (j == vertices_.end()) {
          out << last_tag;
          indentItem += tags::last_skip_item();
        } else {
          out << tag;
          indentItem += tags::skip_item();
        }
        // out << "Vertex ID=" << (*i)->get_hit_id() << " @ "
        //     << (*i)->get_placement().get_translation() / CLHEP::mm << " mm"
        //     << " (" << (*i)->get_auxiliaries().fetch_string(vertex_type_key())
        //     << ")" << std::endl;
        out << "Vertex ID=" << (*i)->get_hit_id() << " : " << std::endl;
        (*i)->tree_dump(out, "", indent + indentItem);
      }

      // >>>
      out << indent << tag
          << "Associated calorimeter hit(s) : ";
      if (has_associated_calorimeter_hits()) {
        out << associated_calorimeters_.size();
      } else {
        out << "<No>";
      }
      out << std::endl;
      for (auto i = associated_calorimeters_.begin();
           i != associated_calorimeters_.end(); i++) {
        out << indent << tag;
        auto j = i;
        j++;
        if (j == associated_calorimeters_.end()) {
          out << last_tag;
        } else {
          out << tag;
        }
        out << "Hit Id=" << (*i)->get_hit_id() << " @ "
            << (*i)->get_geom_id() << std::endl;
      }
      // <<<

      out << indent << inherit_tag(popts.inherit)
          << "Calorimeter hit/vertex association(s) : ";
      if (calo_vertex_associations_.size()) {
        out << calo_vertex_associations_.size();
      } else {
        out << "<No>";
      }
      out << std::endl;
     for (auto i = calo_vertex_associations_.begin();
           i != calo_vertex_associations_.end(); i++) {
        out << indent << inherit_skip_tag(popts.inherit);
        auto j = i;
        j++;
        if (j == calo_vertex_associations_.end()) {
          out << last_tag;
        } else {
          out << tag;
        }
        out << "Association = " << (*i).vertex->get_hit_id() << " <-> "
            << (*i).hit->get_hit_id() << "(GID=" << (*i).hit->get_geom_id() << ")" << std::endl;
      }
      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(particle_track,
                                                      "snemo::datamodel::particle_track")

  } // end of namespace datamodel

} // end of namespace snemo

// end 
