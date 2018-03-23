/// \file falaise/snemo/datamodels/sim_tracker_digi_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/sim_tracker_digi_hit.h>

namespace snemo {

namespace datamodel {

// Serial tag :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(sim_tracker_digi_hit,
                                                  "snemo::datamodel::sim_tracker_digi_hit")

sim_tracker_digi_hit::sim_tracker_digi_hit() { return; }

sim_tracker_digi_hit::~sim_tracker_digi_hit() { return; }

bool sim_tracker_digi_hit::is_valid() const {
  if (!has_hit_id()) {
    return false;
  }
  if (!has_geom_id()) {
    return false;
  }
  if (!has_anode_t0()) {
    return false;
  }
  return true;
}

void sim_tracker_digi_hit::reset() {
  reset_anode_t0();
  reset_anode_t1();
  reset_anode_t2();
  reset_anode_t3();
  reset_anode_t4();
  reset_cathode_t5();
  reset_cathode_t6();
  return;
}

bool sim_tracker_digi_hit::has_anode_t0() const { return (_store & STORE_ANODE_T0); }

bool sim_tracker_digi_hit::has_anode_t1() const { return (_store & STORE_ANODE_T1); }

bool sim_tracker_digi_hit::has_anode_t2() const { return (_store & STORE_ANODE_T3); }

bool sim_tracker_digi_hit::has_anode_t3() const { return (_store & STORE_ANODE_T3); }

bool sim_tracker_digi_hit::has_anode_t4() const { return (_store & STORE_ANODE_T4); }

bool sim_tracker_digi_hit::has_cathode_t5() const { return (_store & STORE_CATHODE_T5); }

bool sim_tracker_digi_hit::has_cathode_t6() const { return (_store & STORE_CATHODE_T6); }

void sim_tracker_digi_hit::set_anode_t0(int16_t t_) {
  _store_set(STORE_ANODE_T0);
  _anode_t0_ = t_;
  return;
}

void sim_tracker_digi_hit::reset_anode_t0() {
  _store_unset(STORE_ANODE_T0);
  _anode_t0_ = INVALID_TIME;
  return;
}

int16_t sim_tracker_digi_hit::get_anode_t0() const {
  DT_THROW_IF(!has_anode_t0(), std::logic_error, "Missing anode t0");
  return _anode_t0_;
}

void sim_tracker_digi_hit::set_anode_t1(int16_t t_) {
  _store_set(STORE_ANODE_T1);
  _anode_t1_ = t_;
  return;
}

void sim_tracker_digi_hit::reset_anode_t1() {
  _store_unset(STORE_ANODE_T1);
  _anode_t1_ = INVALID_TIME;
  return;
}

int16_t sim_tracker_digi_hit::get_anode_t1() const {
  DT_THROW_IF(!has_anode_t1(), std::logic_error, "Missing anode t1");
  return _anode_t1_;
}

void sim_tracker_digi_hit::set_anode_t2(int16_t t_) {
  _store_set(STORE_ANODE_T2);
  _anode_t2_ = t_;
  return;
}

void sim_tracker_digi_hit::reset_anode_t2() {
  _store_unset(STORE_ANODE_T2);
  _anode_t2_ = INVALID_TIME;
  return;
}

int16_t sim_tracker_digi_hit::get_anode_t2() const {
  DT_THROW_IF(!has_anode_t2(), std::logic_error, "Missing anode t2");
  return _anode_t2_;
}

void sim_tracker_digi_hit::set_anode_t3(int16_t t_) {
  _store_set(STORE_ANODE_T3);
  _anode_t3_ = t_;
  return;
}

void sim_tracker_digi_hit::reset_anode_t3() {
  _store_unset(STORE_ANODE_T3);
  _anode_t3_ = INVALID_TIME;
  return;
}

int16_t sim_tracker_digi_hit::get_anode_t3() const {
  DT_THROW_IF(!has_anode_t3(), std::logic_error, "Missing anode t3");
  return _anode_t3_;
}

void sim_tracker_digi_hit::set_anode_t4(int16_t t_) {
  _store_set(STORE_ANODE_T4);
  _anode_t4_ = t_;
  return;
}

void sim_tracker_digi_hit::reset_anode_t4() {
  _store_unset(STORE_ANODE_T4);
  _anode_t4_ = INVALID_TIME;
  return;
}

int16_t sim_tracker_digi_hit::get_anode_t4() const {
  DT_THROW_IF(!has_anode_t4(), std::logic_error, "Missing anode t4");
  return _anode_t4_;
}

void sim_tracker_digi_hit::set_cathode_t5(int16_t t_) {
  _store_set(STORE_CATHODE_T5);
  _cathode_t5_ = t_;
  return;
}

void sim_tracker_digi_hit::reset_cathode_t5() {
  _store_unset(STORE_CATHODE_T5);
  _cathode_t5_ = INVALID_TIME;
  return;
}

int16_t sim_tracker_digi_hit::get_cathode_t5() const {
  DT_THROW_IF(!has_cathode_t5(), std::logic_error, "Missing cathode t5");
  return _cathode_t5_;
}

void sim_tracker_digi_hit::set_cathode_t6(int16_t t_) {
  _store_set(STORE_CATHODE_T6);
  _cathode_t6_ = t_;
  return;
}

void sim_tracker_digi_hit::reset_cathode_t6() {
  _store_unset(STORE_CATHODE_T6);
  _cathode_t6_ = INVALID_TIME;
  return;
}

int16_t sim_tracker_digi_hit::get_cathode_t6() const {
  DT_THROW_IF(!has_cathode_t6(), std::logic_error, "Missing cathode t6");
  return _cathode_t6_;
}

void sim_tracker_digi_hit::tree_dump(std::ostream& out_, const std::string& title_,
                                     const std::string& indent_, bool inherit_) const {
  this->geomtools::base_hit::tree_dump(out_, title_, indent_, true);

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t0 : ";
  if (has_anode_t0()) {
    out_ << _anode_t0_;
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t1 : ";
  if (has_anode_t1()) {
    out_ << _anode_t1_;
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t2 : ";
  if (has_anode_t2()) {
    out_ << _anode_t2_;
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t3 : ";
  if (has_anode_t3()) {
    out_ << _anode_t3_;
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Anode t4 : ";
  if (has_anode_t4()) {
    out_ << _anode_t4_;
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Cathode t5 : ";
  if (has_cathode_t5()) {
    out_ << _cathode_t5_;
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Cathode t6 : ";
  if (has_cathode_t6()) {
    out_ << _cathode_t6_;
  } else {
    out_ << "<none>";
  }
  out_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Validity : " << (is_valid() ? "<yes>" : "<no>") << std::endl;

  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo
