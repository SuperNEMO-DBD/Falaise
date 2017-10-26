/** \file falaise/snemo/datamodels/topology_1eNg_pattern.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/topology_1eNg_pattern.h>

#include <falaise/snemo/datamodels/energy_measurement.h>
namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(topology_1eNg_pattern,
                                                  "snemo::datamodel::topology_1eNg_pattern")

// static
const std::string& topology_1eNg_pattern::pattern_id() {
  static const std::string _id("1eNg");
  return _id;
}

std::string topology_1eNg_pattern::get_pattern_id() const {
  return topology_1eNg_pattern::pattern_id();
}

topology_1eNg_pattern::topology_1eNg_pattern() : topology_1e_pattern() {
  _number_of_gammas_ = 0;
  return;
}

topology_1eNg_pattern::~topology_1eNg_pattern() { return; }

bool topology_1eNg_pattern::has_number_of_gammas() const { return _number_of_gammas_ != 0; }

void topology_1eNg_pattern::set_number_of_gammas(const size_t ngammas_) {
  _number_of_gammas_ = ngammas_;
  return;
}

size_t topology_1eNg_pattern::get_number_of_gammas() const { return _number_of_gammas_; }

bool topology_1eNg_pattern::has_gammas_energies() const {
  return has_measurement("energy_g[0-9]+");
}

void topology_1eNg_pattern::fetch_gammas_energies(
    topology_1eNg_pattern::energy_collection_type& energies_) const {
  DT_THROW_IF(!has_gammas_energies(), std::logic_error, "No gammas energy measurement stored !");
  for (size_t ig = 1; ig <= get_number_of_gammas(); ig++) {
    std::ostringstream oss;
    oss << "energy_g" << ig;
    DT_THROW_IF(!has_measurement_as<snemo::datamodel::energy_measurement>(oss.str()),
                std::logic_error, "Missing '" << oss.str() << "' energy measurement !");
    const snemo::datamodel::energy_measurement& a_energy_meas =
        get_measurement_as<snemo::datamodel::energy_measurement>(oss.str());
    energies_.push_back(a_energy_meas.get_energy());
  }
  return;
}

bool topology_1eNg_pattern::has_electron_gammas_tof_probabilities() const {
  return has_measurement("tof_e1_g[0-9]+");
}

void topology_1eNg_pattern::fetch_electron_gammas_internal_probabilities(
    topology_1eNg_pattern::tof_collection_type& eg_pint_) const {
  DT_THROW_IF(!has_electron_gammas_tof_probabilities(), std::logic_error,
              "No electron-gammas TOF measurement stored !");
  for (size_t ig = 1; ig <= get_number_of_gammas(); ig++) {
    std::ostringstream oss;
    oss << "tof_e1_g" << ig;
    DT_THROW_IF(!has_measurement_as<snemo::datamodel::tof_measurement>(oss.str()), std::logic_error,
                "Missing '" << oss.str() << "' TOF measurement !");
    const snemo::datamodel::tof_measurement& a_tof_meas =
        get_measurement_as<snemo::datamodel::tof_measurement>(oss.str());
    eg_pint_.push_back(a_tof_meas.get_internal_probabilities());
  }
  return;
}

void topology_1eNg_pattern::fetch_electron_gammas_external_probabilities(
    topology_1eNg_pattern::tof_collection_type& eg_pext_) const {
  DT_THROW_IF(!has_electron_gammas_tof_probabilities(), std::logic_error,
              "No electron-gammas TOF measurement stored !");
  for (size_t ig = 1; ig <= get_number_of_gammas(); ++ig) {
    std::ostringstream oss;
    oss << "tof_e1_g" << ig;
    DT_THROW_IF(!has_measurement_as<snemo::datamodel::tof_measurement>(oss.str()), std::logic_error,
                "Missing '" << oss.str() << "' TOF measurement !");
    const snemo::datamodel::tof_measurement& a_tof_meas =
        get_measurement_as<snemo::datamodel::tof_measurement>(oss.str());
    eg_pext_.push_back(a_tof_meas.get_external_probabilities());
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
