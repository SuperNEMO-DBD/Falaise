/** \file falaise/snemo/datamodels/base_topology_pattern.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/base_topology_pattern.h>

// Standard library:
#include <regex>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_topology_pattern,
                                                  "snemo::datamodel::base_topology_pattern")

base_topology_pattern::base_topology_pattern() { return; }

base_topology_pattern::~base_topology_pattern() { return; }

snemo::datamodel::base_topology_pattern::particle_track_dict_type&
base_topology_pattern::grab_particle_track_dictionary() {
  return _tracks_;
}

const snemo::datamodel::base_topology_pattern::particle_track_dict_type&
base_topology_pattern::get_particle_track_dictionary() const {
  return _tracks_;
}

bool base_topology_pattern::has_particle_track(const std::string& key_) const {
  return _tracks_.find(key_) != _tracks_.end();
}

const snemo::datamodel::particle_track& base_topology_pattern::get_particle_track(
    const std::string& key_) const {
  return _tracks_.at(key_).get();
}

bool base_topology_pattern::has_measurement(const std::string& key_) const {
  // Use key as regular expression and match over it
  auto it = std::find_if(_meas_.begin(), _meas_.end(),
                         [key_](const std::pair<std::string, handle_measurement>& t) -> bool {
                           return std::regex_match(t.first, std::regex(key_));
                         });
  return it != _meas_.end();
}

const snemo::datamodel::base_topology_measurement& base_topology_pattern::get_measurement(
    const std::string& key_) const {
  return _meas_.at(key_).get();
}

snemo::datamodel::base_topology_pattern::measurement_dict_type&
base_topology_pattern::grab_measurement_dictionary() {
  return _meas_;
}

const snemo::datamodel::base_topology_pattern::measurement_dict_type&
base_topology_pattern::get_measurement_dictionary() const {
  return _meas_;
}

void base_topology_pattern::tree_dump(std::ostream& out_, const std::string& title_,
                                      const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::tag << "Pattern ID : "
       << "'" << get_pattern_id() << "'" << std::endl;

  {
    out_ << indent << datatools::i_tree_dumpable::tag << "Associated particle tracks : ";
    if (_tracks_.empty()) {
      out_ << "<none>";
    } else {
      out_ << _tracks_.size();
    }
    out_ << std::endl;
    for (snemo::datamodel::base_topology_pattern::particle_track_dict_type::const_iterator i =
             _tracks_.begin();
         i != _tracks_.end(); ++i) {
      const std::string& a_name = i->first;
      const snemo::datamodel::particle_track& a_track = i->second.get();
      out_ << indent << datatools::i_tree_dumpable::skip_tag;
      snemo::datamodel::base_topology_pattern::particle_track_dict_type::const_iterator j = i;
      std::ostringstream indent2;
      indent2 << indent << datatools::i_tree_dumpable::skip_tag;
      if (++j == _tracks_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
        indent2 << datatools::i_tree_dumpable::last_skip_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
        indent2 << datatools::i_tree_dumpable::skip_tag;
      }
      out_ << "Particle '" << a_name << "' :" << std::endl;
      a_track.tree_dump(out_, "", indent2.str());
    }
  }

  {
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Associated measurements : ";
    if (_meas_.empty()) {
      out_ << "<none>";
    } else {
      out_ << _meas_.size();
    }
    out_ << std::endl;
    for (snemo::datamodel::base_topology_pattern::measurement_dict_type::const_iterator i =
             _meas_.begin();
         i != _meas_.end(); ++i) {
      const std::string& a_name = i->first;
      const snemo::datamodel::base_topology_measurement& a_meas = i->second.get();
      out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      snemo::datamodel::base_topology_pattern::measurement_dict_type::const_iterator j = i;
      std::ostringstream indent2;
      indent2 << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      if (++j == _meas_.end()) {
        out_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
        indent2 << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      } else {
        out_ << datatools::i_tree_dumpable::tag;
        indent2 << datatools::i_tree_dumpable::skip_tag;
      }
      out_ << "Measurement '" << a_name << "' :" << std::endl;
      a_meas.tree_dump(out_, "", indent2.str());
    }
  }

  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
