// particle_track_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/particle_track_data.h>

namespace snemo {

  namespace datamodel {

    ParticleHdlCollection get_particles_by_charge(const particle_track_data & ptd,
                                                  uint32_t charge_types)
    {
      ParticleHdlCollection selection;

      for (const auto & a_particle : ptd.particles()) {
        const bool has_negative_charge =
          ((charge_types & particle_track::CHARGE_NEGATIVE) != 0u) && particle_has_negative_charge(*a_particle);
        const bool has_positive_charge =
          ((charge_types & particle_track::CHARGE_POSITIVE) != 0u) && particle_has_positive_charge(*a_particle);
        const bool has_undefined_charge =
          ((charge_types & particle_track::CHARGE_UNDEFINED) != 0u) && particle_has_undefined_charge(*a_particle);
        const bool has_neutral_charge =
          ((charge_types & particle_track::CHARGE_NEUTRAL) != 0u) && particle_has_neutral_charge(*a_particle);

        if (has_negative_charge || has_positive_charge || has_undefined_charge || has_neutral_charge) {
          selection.push_back(a_particle);
        }
      }
      return selection;
    }

    bool particle_track_data::hasParticles() const
    {
      return !particles_.empty();
    }

    size_t particle_track_data::numberOfParticles() const
    {
      return particles_.size();
    }

    void particle_track_data::insertParticle(const ParticleHdl & particle)
    {
      // "Duplicated" means same address:
      DT_THROW_IF(ownsParticle(particle), std::logic_error, "Duplicated particles not allowed!");
      particles_.push_back(particle);
      return;
    }

    ParticleHdlCollection & particle_track_data::particles()
    {
      return particles_;
    }

    const ParticleHdlCollection & particle_track_data::particles() const
    {
      return particles_;
    }

    void particle_track_data::clearParticles()
    {
      particles_.clear();
    }

    bool particle_track_data::ownsParticle(const ParticleHdl & particle) const
    {
      for (const auto & hPart : particles_) {
        // Check addresses:
        if (&*hPart == &*particle) return true;
      }
      return false;
    }

    bool particle_track_data::hasAlternativeTracks(const ParticleHdl & particle) const
    {
      if (not particle->has_trajectory()) {
        return false;
      }
      const auto & hTraj = particle->get_trajectory_handle();
      if (not hTraj->has_cluster()) {
        return false;
      }
      for (const auto & hPart : particles_) {
        if (&*hPart == &*particle) continue; // skip self
        if (hPart->match_cluster(hTraj->get_cluster())) {
          return true;
        }
      }
      return false;
    }
    
    ParticleHdlCollection particle_track_data::alternativeTracks(const ParticleHdl & particle) const
    {
      ParticleHdlCollection alternatives;
      if (particle->has_trajectory()) {
        const auto & hTraj = particle->get_trajectory_handle();
        if (hTraj->has_cluster()) {
          for (const auto & hPart : particles_) {
            if (&*hPart == &*particle) continue; // skip self
            if (hPart->match_cluster(hTraj->get_cluster())) {
              alternatives.push_back(hPart);
            }
          }
        }
      }
      return alternatives;
    }

    bool particle_track_data::hasIsolatedCalorimeters() const
    {
      return !isolated_calorimeters_.empty();
    }

    CalorimeterHitHdlCollection & particle_track_data::isolatedCalorimeters()
    {
      return isolated_calorimeters_;
    }

    const CalorimeterHitHdlCollection & particle_track_data::isolatedCalorimeters() const
    {
      return isolated_calorimeters_;
    }

    void particle_track_data::clearIsolatedCalorimeters()
    {
      isolated_calorimeters_.clear();
    }

    void particle_track_data::clear()
    {
      particles_.clear();
      isolated_calorimeters_.clear();
      return;
    }
    
    void particle_track_data::print_tree(std::ostream & out,
                                         const boost::property_tree::ptree & options) const
    {
      base_print_options popts;
      popts.configure_from(options);
      const std::string & indent = popts.indent;

      if (!popts.title.empty()) {
        out << indent << popts.title << std::endl;
      }

      out << indent << tag << "Particle(s) : " << particles_.size()
          << std::endl;

      for (size_t i = 0; i < numberOfParticles(); i++) {
        std::ostringstream indent2;
        out << indent << skip_tag;
        indent2 << indent << skip_tag;
        if (i == numberOfParticles() - 1) {
          out << last_tag;
          indent2 << last_skip_tag;
        } else {
          out << tag;
          indent2 << skip_tag;
        }
        out << "Particle #" << i << " : " << std::endl;
        {
          boost::property_tree::ptree partPopts;
          partPopts.put("indent", indent2.str());
          particles_[i]->print_tree(out, partPopts);
        }
      }

      const auto & the_calos = isolatedCalorimeters();
      out << indent << inherit_tag(popts.inherit) 
          << "Isolated calorimeter(s) : " << the_calos.size() << std::endl;
      for (size_t i = 0; i < the_calos.size(); i++) {
        const calibrated_calorimeter_hit & a_calo_hit = the_calos.at(i).get();
        std::ostringstream indent2;
        out << indent << inherit_skip_tag(popts.inherit);
        indent2 << indent << inherit_skip_tag(popts.inherit);
        if (i == the_calos.size() - 1) {
          out << last_tag;
          indent2 << last_skip_tag;
        } else {
          out << tag;
          indent2 << skip_tag;
        }
        out << "Hit Id=" << a_calo_hit.get_hit_id() << " @ " << a_calo_hit.get_geom_id();
        out << std::endl;
      }
      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(particle_track_data,
                                                      "snemo::datamodel::particle_track_data")

  } // namespace datamodel

} // end of namespace snemo
