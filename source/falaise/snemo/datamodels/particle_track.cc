// falaise/snemo/datamodels/particle_track.cc

// Ourselves:
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

namespace datamodel {

bool particle_has(const Particle& p, Particle::charge_type charge) {
  return p.get_charge() == charge;
}

bool particle_has_negative_charge(const Particle& p) { return particle_has(p, Particle::NEGATIVE); }

bool particle_has_positive_charge(const Particle& p) { return particle_has(p, Particle::POSITIVE); }

bool particle_has_undefined_charge(const Particle& p) {
  return particle_has(p, Particle::UNDEFINED);
}

bool particle_has_neutral_charge(const Particle& p) { return particle_has(p, Particle::NEUTRAL); }

const std::string& particle_track::vertex_type_key() {
  static const std::string _flag("vertex.type");
  return _flag;
}

const std::string& particle_track::vertex_type_to_label(vertex_type v) {
  switch (v) {
    case VERTEX_ON_SOURCE_FOIL:
      return vertex_on_source_foil_label();
    case VERTEX_ON_MAIN_CALORIMETER:
      return vertex_on_main_calorimeter_label();
    case VERTEX_ON_X_CALORIMETER:
      return vertex_on_x_calorimeter_label();
    case VERTEX_ON_GAMMA_VETO:
      return vertex_on_gamma_veto_label();
    case VERTEX_ON_WIRE:
      return vertex_on_wire_label();
    default:
      return vertex_none_label();
  }
}

particle_track::vertex_type particle_track::label_to_vertex_type(const std::string& label) {
  if (label == vertex_on_source_foil_label()) {
    return VERTEX_ON_SOURCE_FOIL;
  }
  if (label == vertex_on_main_calorimeter_label()) {
    return VERTEX_ON_MAIN_CALORIMETER;
  }
  if (label == vertex_on_x_calorimeter_label()) {
    return VERTEX_ON_X_CALORIMETER;
  }
  if (label == vertex_on_gamma_veto_label()) {
    return VERTEX_ON_GAMMA_VETO;
  }
  if (label == vertex_on_wire_label()) {
    return VERTEX_ON_WIRE;
  }
  return VERTEX_NONE;
}

const std::string& particle_track::vertex_none_label() {
  static const std::string _flag("!");
  return _flag;
}

const std::string& particle_track::vertex_on_wire_label() {
  static const std::string _flag("wire");
  return _flag;
}

const std::string& particle_track::vertex_on_source_foil_label() {
  static const std::string _flag("foil");
  return _flag;
}

const std::string& particle_track::vertex_on_main_calorimeter_label() {
  static const std::string _flag("calo");
  return _flag;
}

const std::string& particle_track::vertex_on_x_calorimeter_label() {
  static const std::string _flag("xcalo");
  return _flag;
}

const std::string& particle_track::vertex_on_gamma_veto_label() {
  static const std::string _flag("gveto");
  return _flag;
}

bool particle_track::vertex_is(const geomtools::blur_spot& vtx, vertex_type vtype) {
  if (vtx.get_auxiliaries().has_key(vertex_type_key())) {
    std::string vtype_label = vtx.get_auxiliaries().fetch_string(vertex_type_key());
    return vtype_label == vertex_type_to_label(vtype);
  }
  return vtype == VERTEX_NONE;
}

bool particle_track::vertex_is_on_source_foil(const geomtools::blur_spot& vtx) {
  return vertex_is(vtx, VERTEX_ON_SOURCE_FOIL);
}

bool particle_track::vertex_is_on_main_calorimeter(const geomtools::blur_spot& vtx) {
  return vertex_is(vtx, VERTEX_ON_MAIN_CALORIMETER);
}

bool particle_track::vertex_is_on_x_calorimeter(const geomtools::blur_spot& vtx) {
  return vertex_is(vtx, VERTEX_ON_X_CALORIMETER);
}

bool particle_track::vertex_is_on_gamma_veto(const geomtools::blur_spot& vtx) {
  return vertex_is(vtx, VERTEX_ON_GAMMA_VETO);
}

bool particle_track::vertex_is_on_wire(const geomtools::blur_spot& vtx) {
  return vertex_is(vtx, VERTEX_ON_WIRE);
}

bool particle_track::has_track_id() const { return has_hit_id(); }

int particle_track::get_track_id() const { return get_hit_id(); }

void particle_track::set_track_id(int32_t id) { set_hit_id(id); }

void particle_track::set_charge(charge_type charge) { charge_from_source_ = charge; }

particle_track::charge_type particle_track::get_charge() const { return charge_from_source_; }

bool particle_track::has_trajectory() const { return trajectory_.has_data(); }

void particle_track::detach_trajectory() { trajectory_.reset(); }

TrackerTrajectoryHdl& particle_track::get_trajectory_handle() { return trajectory_; }

const TrackerTrajectoryHdl& particle_track::get_trajectory_handle() const { return trajectory_; }

void particle_track::set_trajectory_handle(const TrackerTrajectoryHdl& trajectory) {
  trajectory_ = trajectory;
}

tracker_trajectory& particle_track::get_trajectory() { return *trajectory_; }

const tracker_trajectory& particle_track::get_trajectory() const { return *trajectory_; }

bool particle_track::has_vertices() const { return !vertices_.empty(); }

void particle_track::clear_vertices() { vertices_.clear(); }

particle_track::vertex_collection_type& particle_track::get_vertices() { return vertices_; }

const particle_track::vertex_collection_type& particle_track::get_vertices() const {
  return vertices_;
}

bool particle_track::has_associated_calorimeter_hits() const {
  return !associated_calorimeters_.empty();
}

void particle_track::clear_associated_calorimeter_hits() { associated_calorimeters_.clear(); }

CalorimeterHitHdlCollection& particle_track::get_associated_calorimeter_hits() {
  return associated_calorimeters_;
}

const CalorimeterHitHdlCollection& particle_track::get_associated_calorimeter_hits() const {
  return associated_calorimeters_;
}

void particle_track::clear() {
  clear_vertices();
  clear_associated_calorimeter_hits();
  detach_trajectory();
  base_hit::clear();
  charge_from_source_ = UNDEFINED;
}

void particle_track::tree_dump(std::ostream& out, const std::string& title,
                               const std::string& indent, bool is_last) const {
  base_hit::tree_dump(out, title, indent, true);

  out << indent << datatools::i_tree_dumpable::tag << "Track ID : " << get_track_id() << std::endl;

  out << indent << datatools::i_tree_dumpable::tag << "Trajectory : ";
  if (has_trajectory()) {
    out << trajectory_->get_id();
  } else {
    out << "<none>";
  }
  out << std::endl;

  out << indent << datatools::i_tree_dumpable::tag << "Particle charge : ";
  if (get_charge() == UNDEFINED) {
    out << "invalid";
  } else if (get_charge() == NEGATIVE) {
    out << "negative";
  } else if (get_charge() == POSITIVE) {
    out << "positive";
  } else if (get_charge() == NEUTRAL) {
    out << "neutral";
  }
  out << std::endl;

  out << indent << datatools::i_tree_dumpable::tag << "Vertices[" << vertices_.size()
      << "] :" << std::endl;
  for (auto i = vertices_.begin(); i != vertices_.end(); i++) {
    out << indent << datatools::i_tree_dumpable::skip_tag;
    auto j = i;
    j++;
    if (j == vertices_.end()) {
      out << datatools::i_tree_dumpable::last_tag;
    } else {
      out << datatools::i_tree_dumpable::tag;
    }
    out << "Vertex Id=" << (*i)->get_hit_id() << " @ "
        << (*i)->get_placement().get_translation() / CLHEP::mm << " mm"
        << " (" << (*i)->get_auxiliaries().fetch_string(vertex_type_key()) << ")" << std::endl;
  }

  out << indent << datatools::i_tree_dumpable::inherit_tag(is_last)
      << "Associated calorimeter hit(s) : ";
  if (has_associated_calorimeter_hits()) {
    out << associated_calorimeters_.size();
  } else {
    out << "<No>";
  }
  out << std::endl;
  for (auto i = associated_calorimeters_.begin(); i != associated_calorimeters_.end(); i++) {
    out << indent << datatools::i_tree_dumpable::inherit_skip_tag(is_last);
    auto j = i;
    j++;
    if (j == associated_calorimeters_.end()) {
      out << datatools::i_tree_dumpable::last_tag;
    } else {
      out << datatools::i_tree_dumpable::tag;
    }
    out << "Hit Id=" << (*i)->get_hit_id() << " @ " << (*i)->get_geom_id() << std::endl;
  }
}

// serial tag for datatools::serialization::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(particle_track,
                                                  "snemo::datamodel::particle_track")

}  // end of namespace datamodel

}  // end of namespace snemo

// end of falaise/snemo/datamodels/particle_track.cc
