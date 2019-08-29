// particle_track_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/particle_track_data.h>

namespace snemo {

namespace datamodel {

bool particle_track_data::hasParticles() const { return !_particles_.empty(); }

size_t particle_track_data::numberOfParticles() const { return _particles_.size(); }

void particle_track_data::insertParticle(const ParticleHdl& particle_handle_) {
  for (const auto& h : _particles_) {
    // "Duplicated" means same address
    DT_THROW_IF((&*h == &*particle_handle_), std::logic_error, "Duplicated particles not allowed!");
  }
  _particles_.push_back(particle_handle_);
}

ParticleHdlCollection& particle_track_data::particles() { return _particles_; }

const ParticleHdlCollection& particle_track_data::particles() const { return _particles_; }

void particle_track_data::clearParticles() { _particles_.clear(); }

ParticleHdlCollection particle_track_data::getParticlesByCharge(const uint32_t charge) const {
  ParticleHdlCollection selection;

  for (const auto& a_particle : particles()) {
    // const particle_track::handle_type& a_particle = *i;
    const bool has_negative_charge = ((charge & particle_track::NEGATIVE) != 0u) &&
                                     particle_track::particle_has_negative_charge(*a_particle);
    const bool has_positive_charge = ((charge & particle_track::POSITIVE) != 0u) &&
                                     particle_track::particle_has_positive_charge(*a_particle);
    const bool has_undefined_charge = ((charge & particle_track::UNDEFINED) != 0u) &&
                                      particle_track::particle_has_undefined_charge(*a_particle);
    const bool has_neutral_charge = ((charge & particle_track::NEUTRAL) != 0u) &&
                                    particle_track::particle_has_neutral_charge(*a_particle);

    if (has_negative_charge || has_positive_charge || has_undefined_charge || has_neutral_charge) {
      selection.push_back(a_particle);
    }
  }
  return selection;
}

bool particle_track_data::hasIsolatedCalorimeters() const {
  return !_non_associated_calorimeters_.empty();
}

calibrated_calorimeter_hit::collection_type& particle_track_data::isolatedCalorimeters() {
  return _non_associated_calorimeters_;
}

const calibrated_calorimeter_hit::collection_type& particle_track_data::isolatedCalorimeters()
    const {
  return _non_associated_calorimeters_;
}

void particle_track_data::clearIsolatedCalorimeters() {
  _non_associated_calorimeters_.clear();
}

void particle_track_data::clear() {
  _particles_.clear();
  _non_associated_calorimeters_.clear();
}

void particle_track_data::tree_dump(std::ostream& out_, const std::string& title_,
                                    const std::string& indent_, bool inherit_) const {
  if (!title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Particle(s) : " << _particles_.size()
       << std::endl;

  for (size_t i = 0; i < numberOfParticles(); i++) {
    std::ostringstream indent2;
    out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
    indent2 << indent_ << datatools::i_tree_dumpable::skip_tag;
    if (i == numberOfParticles() - 1) {
      out_ << datatools::i_tree_dumpable::last_tag;
      indent2 << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
      indent2 << datatools::i_tree_dumpable::skip_tag;
    }
    out_ << "Particle #" << i << " : " << std::endl;
    _particles_[i]->tree_dump(out_, "", indent2.str());
  }

  const auto& the_calos = isolatedCalorimeters();
  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Unassociated calorimeter(s) : " << the_calos.size() << std::endl;
  for (size_t i = 0; i < the_calos.size(); i++) {
    const calibrated_calorimeter_hit& a_calo_hit = the_calos.at(i).get();
    std::ostringstream indent2;
    out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
    indent2 << indent_ << datatools::i_tree_dumpable::skip_tag;
    if (i == the_calos.size() - 1) {
      out_ << datatools::i_tree_dumpable::last_tag;
      indent2 << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
      indent2 << datatools::i_tree_dumpable::skip_tag;
    }
    out_ << "Hit Id=" << a_calo_hit.get_hit_id() << " @ " << a_calo_hit.get_geom_id();
    out_ << std::endl;
  }
}

// serial tag for datatools::serialization::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(particle_track_data,
                                                  "snemo::datamodel::particle_track_data")

}  // namespace datamodel

}  // end of namespace snemo
