// physics_list_utils.cc

// Ourselves:
#include <snsim/physics_list_utils.h>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

// This project:
#include <snsim/base_physics_constructor.h>

namespace snsim {

auto get_builder_type_label(G4BuilderType type_) -> std::string {
  if (type_ == bTransportation) {
    return "transportation";
  }
  if (type_ == bElectromagnetic) {
    return "electromagnetic";
  }
  if (type_ == bEmExtra) {
    return "electromagnetic_extra";
  }
  if (type_ == bDecay) {
    return "decay";
  } else if (type_ == bHadronElastic) {
    return "hadronic_elastic";
  } else if (type_ == bHadronInelastic) {
    return "hadronic_inelastic";
  } else if (type_ == bStopping) {
    return "stopping";
  } else if (type_ == bIons) {
    return "ions";
  } else {
    return "";
  }
}

auto get_builder_type(const std::string& label_) -> G4BuilderType {
  if (label_ == "transportation") {
    return bTransportation;
  }
  if (label_ == "electromagnetic") {
    return bElectromagnetic;
  }
  if (label_ == "electromagnetic_extra") {
    return bEmExtra;
  }
  if (label_ == "decay") {
    return bDecay;
  } else if (label_ == "hadronic_elastic") {
    return bHadronElastic;
  } else if (label_ == "hadronic_inelastic") {
    return bHadronInelastic;
  } else if (label_ == "stopping") {
    return bStopping;
  } else if (label_ == "ions") {
    return bIons;
  } else {
    return bUnknown;
  }
}

// *** physics_constructor_entry *** //

physics_constructor_entry::physics_constructor_entry() {
  name = "";
  id = "";
  status = 0;
  handle.reset(nullptr);
}

auto physics_constructor_entry::get_physics_constructor() const -> const base_physics_constructor& {
  DT_THROW_IF(!(status & STATUS_CREATED), std::logic_error,
              "Physics constructor '" << name << "' with ID '" << id << "' is not created !");
  return handle.get();
}

auto physics_constructor_entry::grab_physics_constructor() -> base_physics_constructor& {
  DT_THROW_IF(!(status & STATUS_CREATED), std::logic_error,
              "Physics constructor '" << name << "' with ID '" << id << "' is not created !");
  return handle.grab();
}

// *** physics_constructor_proxy *** //
physics_constructor_proxy::physics_constructor_proxy(base_physics_constructor& pc_) { pc = &pc_; }

physics_constructor_proxy::~physics_constructor_proxy() { pc = nullptr; }

void physics_constructor_proxy::ConstructParticle() { pc->ConstructParticle(); }

void physics_constructor_proxy::ConstructProcess() { pc->ConstructProcess(); }

// *** physics_list_proxy *** //
physics_list_proxy::physics_list_proxy(G4VModularPhysicsList& pl_) { pl = &pl_; }

physics_list_proxy::~physics_list_proxy() { pl = nullptr; }

void physics_list_proxy::ConstructParticle() { pl->ConstructParticle(); }

void physics_list_proxy::ConstructProcess() { pl->ConstructProcess(); }

void physics_list_proxy::SetCuts() { pl->SetCuts(); }

// *** user_limits_info *** //

user_limits_info::user_limits_info() { reset(); }

void user_limits_info::reset() {
  datatools::invalidate(step_max_length);
  datatools::invalidate(track_max_length);
  datatools::invalidate(track_max_time);
  datatools::invalidate(track_min_kinetic_energy);
  datatools::invalidate(track_min_range);
}

void user_limits_info::make_step_limitation(double step_max_length_) {
  reset();

  DT_THROW_IF(step_max_length_ <= 0.0, std::domain_error,
              "Invalid step max lenght (" << step_max_length_ << ") !");
  step_max_length = step_max_length_;
}

void user_limits_info::make_track_limitation(double track_max_length_, double track_max_time_,
                                             double track_min_kinetic_energy_,
                                             double track_min_range_) {
  reset();

  DT_THROW_IF(track_max_length_ <= 0.0, std::domain_error,
              "Invalid track max length (" << track_max_length_ << ") !");
  track_max_length = track_max_length_;

  DT_THROW_IF(track_max_time_ <= 0.0, std::domain_error,
              "Invalid track max time (" << track_max_time_ << ") !");
  track_max_time = track_max_time_;

  DT_THROW_IF(track_min_kinetic_energy_ <= 0.0, std::domain_error,
              "Invalid track min kinetic energy (" << track_min_kinetic_energy_ << ") !");
  track_min_kinetic_energy = track_min_kinetic_energy_;

  DT_THROW_IF(track_min_range_ <= 0.0, std::domain_error,
              "Invalid track min range (" << track_min_range_ << ") !");
  track_min_range = track_min_range_;
}

}  // namespace snsim
