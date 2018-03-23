// snemo/digitization/tracker_feb_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>

#include "tracker_feb_data.h"
#include <iostream>

using namespace std;

namespace snemo {
namespace digitization {

tracker_feb_data::tracker_feb_data() {  // D-fault C-tor
  reset();
}

bool tracker_feb_data::is_initialized() const { return _initialized_; }

void tracker_feb_data::initialize(bool operating_mode_) {
  // operating mode ALTERNATIVE / NEMO3 == [ 0 / 1 ]
  _data_description_.set(MODE_STATUS, operating_mode_);
  _initialized_ = true;
}

void tracker_feb_data::reset() {
  _data_description_.reset();
  geomtools::base_hit::reset();
  anodic_t0 = datatools::invalid_real();  // YL : TO CHECK and continue
  _initialized_ = false;
}

bool tracker_feb_data::has_anodic_t0() const {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "tracker feb data not initialized, can't get anodic status ! ");
  return _data_description_.test(ANODIC_T0_STATUS);
}

bool tracker_feb_data::has_anodic_t1() const {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "tracker feb data not initialized, can't get anodic status ! ");
  return _data_description_.test(ANODIC_T1_STATUS);
}
bool tracker_feb_data::has_anodic_t2() const {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "tracker feb data not initialized, can't get anodic status ! ");
  return _data_description_.test(ANODIC_T2_STATUS);
}
bool tracker_feb_data::has_anodic_t3() const {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "tracker feb data not initialized, can't get anodic status ! ");
  return _data_description_.test(ANODIC_T3_STATUS);
}
bool tracker_feb_data::has_anodic_t4() const {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "tracker feb data not initialized, can't get anodic status ! ");
  return _data_description_.test(ANODIC_T4_STATUS);
}
bool tracker_feb_data::has_cathodic_t5() const {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "tracker feb data not initialized, can't get cathodic status ! ");
  return _data_description_.test(CATHODIC_T5_STATUS);
}
bool tracker_feb_data::has_cathodic_t6() const {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "tracker feb data not initialized, can't get cathodic status ! ");
  return _data_description_.test(CATHODIC_T6_STATUS);
}
bool tracker_feb_data::is_nemo3_mode() {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "tracker feb data not initialized, can't get tracker mode ! ");
  bool status = false;
  if (_data_description_.test(MODE_STATUS) == false)  // NEMO3 mode
    status = true;
  return status;
}

bool tracker_feb_data::is_alternative_mode() {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "tracker feb data not initialized, can't get tracker mode ! ");
  bool status = false;
  if (_data_description_.test(MODE_STATUS) == true)  // ALTERNATIVE mode
    status = true;
  return status;
}

void tracker_feb_data::set_anodic_times(uint64_t t0_, uint64_t t1_, uint64_t t2_, uint64_t t3_,
                                        uint64_t t4_) {
  set_anodic_t0(t0_);
  set_anodic_t1(t1_);
  set_anodic_t2(t2_);
  set_anodic_t3(t3_);
  set_anodic_t4(t4_);
}
void tracker_feb_data::set_cathodic_times(uint64_t t5_, uint64_t t6_) {
  set_cathodic_t5(t5_);
  set_cathodic_t6(t6_);
}
void tracker_feb_data::set_data(uint64_t t0_, uint64_t t1_, uint64_t t2_, uint64_t t3_,
                                uint64_t t4_, uint64_t t5_, uint64_t t6_) {
  set_anodic_times(t0_, t1_, t2_, t3_, t4_);
  set_cathodic_times(t5_, t6_);
}

void tracker_feb_data::set_header(int32_t id_, const geomtools::geom_id& feb_id_) {
  set_hit_id(id_);
  set_geom_id(feb_id_);
}

void tracker_feb_data::display() {
  //  tracker_feb_id.dump();
}

void tracker_feb_data::tree_dump(std::ostream& out_, const std::string& title_,
                                 const std::string& indent_, bool inherit_) const {
  base_hit::tree_dump(out_, title_, indent_, true);

  if (is_initialized()) {
    out_ << indent_ << datatools::i_tree_dumpable::tag << "Anodic time  : " << std::endl;
    if (has_anodic_t0())
      out_ << indent_ << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
           << "t0 = " << anodic_t0 << std::endl;

    if (has_anodic_t1())
      out_ << indent_ << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
           << "t1 = " << anodic_t1 << std::endl;

    if (has_anodic_t2())
      out_ << indent_ << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
           << "t2 = " << anodic_t2 << std::endl;

    if (has_anodic_t3())
      out_ << indent_ << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag
           << "t3 = " << anodic_t3 << std::endl;

    if (has_anodic_t4())
      out_ << indent_ << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::inherit_tag(inherit_) << "t4 = " << anodic_t4
           << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Cathodic time  : " << std::endl;
    if (has_cathodic_t5())
      out_ << indent_ << "    " << datatools::i_tree_dumpable::tag << "t5 = " << cathodic_t5
           << std::endl;
    if (has_cathodic_t6())
      out_ << indent_ << "    " << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "t6 = " << cathodic_t6 << std::endl;
  } else {
    std::clog << "INFO : tracker_feb_data::tree_dump : Current object not initialized !"
              << std::endl;
  }

  return;
}

}  // end of namespace digitization
}  // end of namespace snemo
