// snemo/digitization/geiger_ctw_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/geiger_ctw_data.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {

namespace digitization {

// Serial tag for datatools::serialization::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(geiger_ctw_data,
                                                  "snemo::digitalization::geiger_ctw_data")

geiger_ctw_data::geiger_ctw_data() { return; }

geiger_ctw_data::~geiger_ctw_data() {
  reset();
  return;
}

unsigned int geiger_ctw_data::get_clocktick_min_index() const {
  DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");

  unsigned int index_with_min = 0;

  uint32_t clocktick_min = _geiger_ctws_[0].get().get_clocktick_800ns();

  for (unsigned int i = 1; i < _geiger_ctws_.size(); i++) {
    if (_geiger_ctws_[i].get().get_clocktick_800ns() < clocktick_min) {
      clocktick_min = _geiger_ctws_[i].get().get_clocktick_800ns();
      index_with_min = i;
    }
  }
  return index_with_min;
}

unsigned int geiger_ctw_data::get_clocktick_max_index() const {
  DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");

  unsigned int index_with_max = 0;

  uint32_t clocktick_max = _geiger_ctws_[0].get().get_clocktick_800ns();

  for (unsigned int i = 1; i < _geiger_ctws_.size(); i++) {
    if (_geiger_ctws_[i].get().get_clocktick_800ns() > clocktick_max) {
      clocktick_max = _geiger_ctws_[i].get().get_clocktick_800ns();
      index_with_max = i;
    }
  }
  return index_with_max;
}

uint32_t geiger_ctw_data::get_clocktick_min() const {
  DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
  return _geiger_ctws_[get_clocktick_min_index()].get().get_clocktick_800ns();
}

uint32_t geiger_ctw_data::get_clocktick_max() const {
  DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
  return _geiger_ctws_[get_clocktick_max_index()].get().get_clocktick_800ns();
}

uint32_t geiger_ctw_data::get_clocktick_range() const {
  DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
  return get_clocktick_max() - get_clocktick_min();
}

void geiger_ctw_data::get_list_of_geiger_ctw_per_clocktick(
    uint32_t clocktick_800ns_, geiger_ctw_collection_type& ctws_) const {
  DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
  for (unsigned int i = 0; i < _geiger_ctws_.size(); i++) {
    if (_geiger_ctws_[i].get().get_clocktick_800ns() == clocktick_800ns_ &&
        _geiger_ctws_[i].get().has_trigger_primitive_values()) {
      ctws_.push_back(_geiger_ctws_[i]);
    }
  }
  return;
}

void geiger_ctw_data::reset_ctws() {
  _geiger_ctws_.clear();
  return;
}

geiger_ctw& geiger_ctw_data::add() {
  {
    geiger_ctw_handle_type dummy;
    _geiger_ctws_.push_back(dummy);
  }
  geiger_ctw_handle_type& last = _geiger_ctws_.back();
  last.reset(new geiger_ctw);
  return last.grab();
}

const geiger_ctw_data::geiger_ctw_collection_type& geiger_ctw_data::get_geiger_ctws() const {
  return _geiger_ctws_;
}

geiger_ctw_data::geiger_ctw_collection_type& geiger_ctw_data::grab_geiger_ctws() {
  return _geiger_ctws_;
}

bool geiger_ctw_data::has_geiger_ctw() const {
  if (_geiger_ctws_.empty() == true)
    return false;
  else
    return true;
}

void geiger_ctw_data::reset() {
  reset_ctws();
  return;
}

void geiger_ctw_data::tree_dump(std::ostream& out_, const std::string& title_,
                                const std::string& indent_, bool inherit_) const {
  out_ << indent_ << title_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Geiger CTWs : " << _geiger_ctws_.size() << std::endl;

  return;
}

void geiger_ctw_data::_check() {
  DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
  for (unsigned int i = 0; i < _geiger_ctws_.size() - 1; i++) {
    const geiger_ctw& ctw_a = _geiger_ctws_[i].get();

    for (unsigned int j = i + 1; j < _geiger_ctws_.size(); j++) {
      const geiger_ctw& ctw_b = _geiger_ctws_[j].get();

      DT_THROW_IF(ctw_a.get_clocktick_800ns() == ctw_b.get_clocktick_800ns() &&
                      ctw_a.get_geom_id() == ctw_b.get_geom_id(),
                  std::logic_error,
                  "Duplicate clocktick=" << ctw_a.get_clocktick_800ns() << " * "
                                         << "GID=" << ctw_b.get_geom_id());
    }
  }
  return;
}

}  // end of namespace digitization

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
