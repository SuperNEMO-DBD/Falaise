// particle_track_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/particle_track_data.h>

namespace snemo {

namespace datamodel {

ParticleHdlCollection get_particles_by_charge(const particle_track_data& ptd, uint32_t charge_types) {
  ParticleHdlCollection selection;

  for (const auto& a_particle : ptd.particles()) {
    const bool has_negative_charge =
        ((charge_types & particle_track::NEGATIVE) != 0u) && particle_has_negative_charge(*a_particle);
    const bool has_positive_charge =
        ((charge_types & particle_track::POSITIVE) != 0u) && particle_has_positive_charge(*a_particle);
    const bool has_undefined_charge =
        ((charge_types & particle_track::UNDEFINED) != 0u) && particle_has_undefined_charge(*a_particle);
    const bool has_neutral_charge =
        ((charge_types & particle_track::NEUTRAL) != 0u) && particle_has_neutral_charge(*a_particle);

    if (has_negative_charge || has_positive_charge || has_undefined_charge || has_neutral_charge) {
      selection.push_back(a_particle);
    }
  }
  return selection;
}

bool particle_track_data::hasParticles() const { return !particles_.empty(); }

size_t particle_track_data::numberOfParticles() const { return particles_.size(); }

void particle_track_data::insertParticle(const ParticleHdl& particle) {
  for (const auto& h : particles_) {
    // "Duplicated" means same address
    DT_THROW_IF((&*h == &*particle), std::logic_error, "Duplicated particles not allowed!");
  }
  particles_.push_back(particle);
}

ParticleHdlCollection& particle_track_data::particles() { return particles_; }

const ParticleHdlCollection& particle_track_data::particles() const { return particles_; }

void particle_track_data::clearParticles() { particles_.clear(); }


bool particle_track_data::hasIsolatedCalorimeters() const {
  return !isolated_calorimeters_.empty();
}

CalorimeterHitHdlCollection& particle_track_data::isolatedCalorimeters() {
  return isolated_calorimeters_;
}

const CalorimeterHitHdlCollection& particle_track_data::isolatedCalorimeters() const {
  return isolated_calorimeters_;
}

void particle_track_data::clearIsolatedCalorimeters() { isolated_calorimeters_.clear(); }

void particle_track_data::clear() {
  particles_.clear();
  isolated_calorimeters_.clear();
}

void particle_track_data::tree_dump(std::ostream& out, const std::string& title,
                                    const std::string& indent, bool inherit_) const {
  if (!title.empty()) {
    out << indent << title << std::endl;
  }

  out << indent << datatools::i_tree_dumpable::tag << "Particle(s) : " << particles_.size()
      << std::endl;

  for (size_t i = 0; i < numberOfParticles(); i++) {
    std::ostringstream indent2;
    out << indent << datatools::i_tree_dumpable::skip_tag;
    indent2 << indent << datatools::i_tree_dumpable::skip_tag;
    if (i == numberOfParticles() - 1) {
      out << datatools::i_tree_dumpable::last_tag;
      indent2 << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out << datatools::i_tree_dumpable::tag;
      indent2 << datatools::i_tree_dumpable::skip_tag;
    }
    out << "Particle #" << i << " : " << std::endl;
    particles_[i]->tree_dump(out, "", indent2.str());
  }

  const auto& the_calos = isolatedCalorimeters();
  out << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) 
      << "Isolated calorimeter(s) : " << the_calos.size() << std::endl;
  for (size_t i = 0; i < the_calos.size(); i++) {
    const calibrated_calorimeter_hit& a_calo_hit = the_calos.at(i).get();
    std::ostringstream indent2;
    out << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
    indent2 << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
    if (i == the_calos.size() - 1) {
      out << datatools::i_tree_dumpable::last_tag;
      indent2 << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out << datatools::i_tree_dumpable::tag;
      indent2 << datatools::i_tree_dumpable::skip_tag;
    }
    out << "Hit Id=" << a_calo_hit.get_hit_id() << " @ " << a_calo_hit.get_geom_id();
    out << std::endl;
  }
}

// serial tag for datatools::serialization::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(particle_track_data,
                                                  "snemo::datamodel::particle_track_data")

}  // namespace datamodel

}  // end of namespace snemo
