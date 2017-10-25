/** \file falaise/snemo/datamodels/topology_2eNg_pattern.cc
 */

// Ourselves:
#include <falaise/snemo/datamodels/topology_2eNg_pattern.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(topology_2eNg_pattern,
                                                      "snemo::datamodel::topology_2eNg_pattern")

    // static
    const std::string & topology_2eNg_pattern::pattern_id()
    {
      static const std::string _id("2eNg");
      return _id;
    }

    std::string topology_2eNg_pattern::get_pattern_id() const
    {
      return topology_2eNg_pattern::pattern_id();
    }

    topology_2eNg_pattern::topology_2eNg_pattern()
      : topology_2e_pattern()
    {
      _number_of_gammas_ = 0;
      return;
    }

    topology_2eNg_pattern::~topology_2eNg_pattern()
    {
      return;
    }

    bool topology_2eNg_pattern::has_number_of_gammas() const
    {
      return _number_of_gammas_ != 0;
    }

    void topology_2eNg_pattern::set_number_of_gammas(const size_t ngammas_)
    {
      _number_of_gammas_ = ngammas_;
      return;
    }

    size_t topology_2eNg_pattern::get_number_of_gammas() const
    {
      return _number_of_gammas_;
    }

    bool topology_2eNg_pattern::has_gammas_energies() const
    {
      return has_measurement("energy_g[0-9]+");
    }

    void topology_2eNg_pattern::fetch_gammas_energies(topology_2eNg_pattern::energy_collection_type & g_energies_) const
    {
      DT_THROW_IF(! has_gammas_energies(), std::logic_error,
                  "No gamma energy measurement stored !");
      for (size_t ig = 1; ig <= get_number_of_gammas(); ig++) {
        std::ostringstream oss;
        oss << "energy_g" << ig;
        DT_THROW_IF(! has_measurement_as<snemo::datamodel::energy_measurement>(oss.str()),
                    std::logic_error, "Missing '" << oss.str() << "' energy measurement !");
        const snemo::datamodel::energy_measurement & a_energy_meas
          = get_measurement_as<snemo::datamodel::energy_measurement>(oss.str());
        g_energies_.push_back(a_energy_meas.get_energy());
      }
      return;
    }

    bool topology_2eNg_pattern::has_electrons_gammas_tof_probabilities() const
    {
      return has_measurement("tof_e[0-9]+_g[0-9]+");
    }

    void topology_2eNg_pattern::fetch_electrons_gammas_internal_probabilities(topology_2eNg_pattern::tof_collection_type & eg_pint_) const
    {
      DT_THROW_IF(! has_electrons_gammas_tof_probabilities(), std::logic_error,
                  "No electrons-gammas TOF measurement stored !");
      for (size_t ig = 1; ig <= get_number_of_gammas(); ig++) {
        for (size_t ie = 1; ie <= 2; ie++) {
          std::ostringstream oss;
          oss << "tof_e" << ie << "_g" << ig;
          DT_THROW_IF(! has_measurement_as<snemo::datamodel::tof_measurement>(oss.str()),
                      std::logic_error, "Missing '" << oss.str() << "' TOF measurement !");
          const snemo::datamodel::tof_measurement & a_tof_meas
            = get_measurement_as<snemo::datamodel::tof_measurement>(oss.str());
          eg_pint_.push_back(a_tof_meas.get_internal_probabilities());
        }
      }
      return;
    }

    void topology_2eNg_pattern::fetch_electrons_gammas_external_probabilities(topology_2eNg_pattern::tof_collection_type & eg_pext_) const
    {
      DT_THROW_IF(! has_electrons_gammas_tof_probabilities(), std::logic_error,
                  "No electrons-gammas TOF measurement stored !");
      for (size_t ig = 1; ig <= get_number_of_gammas(); ig++) {
        for (size_t ie = 1; ie <= 2; ie++) {
          std::ostringstream oss;
          oss << "tof_e" << ie << "_g" << ig;
          DT_THROW_IF(! has_measurement_as<snemo::datamodel::tof_measurement>(oss.str()),
                      std::logic_error, "Missing '" << oss.str() << "' TOF measurement !");
          const snemo::datamodel::tof_measurement & a_tof_meas
            = get_measurement_as<snemo::datamodel::tof_measurement>(oss.str());
          eg_pext_.push_back(a_tof_meas.get_external_probabilities());
        }
      }
      return;
    }

    bool topology_2eNg_pattern::has_electron_min_gammas_tof_probabilities() const
    {
      if(get_minimal_energy_electron_name() == "e1")
        return has_measurement("tof_e1_g[0-9]+");
      else
        return has_measurement("tof_e2_g[0-9]+");
    }

    void topology_2eNg_pattern::fetch_electron_min_gammas_internal_probabilities(topology_2eNg_pattern::tof_collection_type & eg_pint_) const
    {
      DT_THROW_IF(! has_electron_min_gammas_tof_probabilities(), std::logic_error,
                  "No electron_min-gammas TOF measurement stored !");
      std::string e_min = get_minimal_energy_electron_name();
      for (size_t ig = 1; ig <= get_number_of_gammas(); ig++) {
          std::ostringstream oss;
          oss << "tof_" << e_min << "_g" << ig;
          DT_THROW_IF(! has_measurement_as<snemo::datamodel::tof_measurement>(oss.str()),
                      std::logic_error, "Missing '" << oss.str() << "' TOF measurement !");
          const snemo::datamodel::tof_measurement & a_tof_meas
            = get_measurement_as<snemo::datamodel::tof_measurement>(oss.str());
          eg_pint_.push_back(a_tof_meas.get_internal_probabilities());
      }
      return;
    }

    void topology_2eNg_pattern::fetch_electron_min_gammas_external_probabilities(topology_2eNg_pattern::tof_collection_type & eg_pext_) const
    {
      DT_THROW_IF(! has_electron_min_gammas_tof_probabilities(), std::logic_error,
                  "No electron_min-gammas TOF measurement stored !");
      std::string e_min = get_minimal_energy_electron_name();
      for (size_t ig = 1; ig <= get_number_of_gammas(); ig++) {
          std::ostringstream oss;
          oss << "tof_" << e_min << "_g" << ig;
          DT_THROW_IF(! has_measurement_as<snemo::datamodel::tof_measurement>(oss.str()),
                      std::logic_error, "Missing '" << oss.str() << "' TOF measurement !");
          const snemo::datamodel::tof_measurement & a_tof_meas
            = get_measurement_as<snemo::datamodel::tof_measurement>(oss.str());
          eg_pext_.push_back(a_tof_meas.get_external_probabilities());
      }
      return;
    }

    bool topology_2eNg_pattern::has_electron_max_gammas_tof_probabilities() const
    {
      if(get_maximal_energy_electron_name() == "e1")
        return has_measurement("tof_e1_g[0-9]+");
      else
        return has_measurement("tof_e2_g[0-9]+");
    }

    void topology_2eNg_pattern::fetch_electron_max_gammas_internal_probabilities(topology_2eNg_pattern::tof_collection_type & eg_pint_) const
    {
      DT_THROW_IF(! has_electron_max_gammas_tof_probabilities(), std::logic_error,
                  "No electron_max-gammas TOF measurement stored !");
      std::string e_max = get_maximal_energy_electron_name();
      for (size_t ig = 1; ig <= get_number_of_gammas(); ig++) {
          std::ostringstream oss;
          oss << "tof_" << e_max << "_g" << ig;
          DT_THROW_IF(! has_measurement_as<snemo::datamodel::tof_measurement>(oss.str()),
                      std::logic_error, "Missing '" << oss.str() << "' TOF measurement !");
          const snemo::datamodel::tof_measurement & a_tof_meas
            = get_measurement_as<snemo::datamodel::tof_measurement>(oss.str());
          eg_pint_.push_back(a_tof_meas.get_internal_probabilities());
      }
      return;
    }

    void topology_2eNg_pattern::fetch_electron_max_gammas_external_probabilities(topology_2eNg_pattern::tof_collection_type & eg_pext_) const
    {
      DT_THROW_IF(! has_electron_max_gammas_tof_probabilities(), std::logic_error,
                  "No electron_max-gammas TOF measurement stored !");
      std::string e_max = get_maximal_energy_electron_name();
      for (size_t ig = 1; ig <= get_number_of_gammas(); ig++) {
          std::ostringstream oss;
          oss << "tof_" << e_max << "_g" << ig;
          DT_THROW_IF(! has_measurement_as<snemo::datamodel::tof_measurement>(oss.str()),
                      std::logic_error, "Missing '" << oss.str() << "' TOF measurement !");
          const snemo::datamodel::tof_measurement & a_tof_meas
            = get_measurement_as<snemo::datamodel::tof_measurement>(oss.str());
          eg_pext_.push_back(a_tof_meas.get_external_probabilities());
      }
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
