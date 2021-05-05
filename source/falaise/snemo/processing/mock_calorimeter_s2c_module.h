// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/mock_calorimeter_s2c_module.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-01-12
 * Last modified: 2014-02-27
 *
 * License:
 *
 * Description:
 *
 *   Mock simulation data processor for calorimeter MC hits
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_MOCK_CALORIMETER_S2C_MODULE_H
#define FALAISE_SNEMO_PROCESSING_MOCK_CALORIMETER_S2C_MODULE_H 1

// Standard library:
#include <map>
#include <string>
#include <vector>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/dpp:
#include <dpp/base_module.h>
// - CLHEP
#include <CLHEP/Units/SystemOfUnits.h>

// This project :
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/processing/calorimeter_regime.h>

#include <falaise/snemo/services/geometry.h>
#include <falaise/snemo/services/service_handle.h>

namespace geomtools {
class manager;
}

namespace mctools {
class simulated_data;
}

namespace snemo {

namespace processing {

/// \brief A mock calibration for SuperNEMO calorimeter hits
class mock_calorimeter_s2c_module : public dpp::base_module {
 public:
  // Because dpp::base_module is insane
  virtual ~mock_calorimeter_s2c_module() { this->reset(); }

  /// Initialization
  virtual void initialize(const datatools::properties& ps, datatools::service_manager& /*unused*/,
                          dpp::module_handle_dict_type& /*unused*/);

  /// Reset
  virtual void reset();

  /// Parse calorimeter regime database file
  void parse_calorimeter_regime_database(const std::string & database_path_);

  // Parse pol3d parameters file
  std::vector<double> parse_pol3d_parameters(const std::string & parameters_path_);

  const CalorimeterModel& get_calorimeter_regime(const geomtools::geom_id & gid);

  /// Data record processing
  virtual process_status process(datatools::things& event);

 private:
  /// Digitize calorimeter hits
  void digitizeHits(const mctools::simulated_data& simdata,
                    snemo::datamodel::CalorimeterHitHdlCollection& calohits);

  /// Calibrate calorimeter hits (energy/time resolution spread)
  void calibrateHits(snemo::datamodel::CalorimeterHitHdlCollection& calohits);

  /// Apply basic trigger filter
  void triggerHits(snemo::datamodel::CalorimeterHitHdlCollection& calohits);

  /// Main process function
  void process_impl(const mctools::simulated_data& simdata,
                    snemo::datamodel::CalorimeterHitHdlCollection& calohits);

 private:
  snemo::service_handle<snemo::geometry_svc> geoManager{};  //!< The geometry manager
  mygsl::rng RNG_{};                     //!< PRN generator
  std::vector<std::string> caloTypes{};  //!< Calorimeter hit categories
  typedef std::map<geomtools::geom_id, CalorimeterModel> CaloModelMap;
  CaloModelMap caloModels{};            //!< Calorimeter regime tools
  std::string sdInputTag{};             //!< The label of the simulated data bank
  std::string cdOutputTag{};            //!< The label of the calibrated data bank
  double timeWindow{100. * CLHEP::ns};  //!< Time width of a calo cluster
  bool quenchAlphas{true};              //!< Flag to (dis)activate the alpha quenching
  bool assocMCHitId{false};             //!< The flag to reference MC true hit
  bool _alpha_quenching_;                             //!< Flag to (dis)activate the alpha quenching
  std::vector<double> _uniformity_correction_parameters_mwall_8inch_{1,1}; //!< Polynomial parameters for the uniformity correction for MWall 8"
  std::vector<double> _uniformity_correction_parameters_mwall_5inch_{1,1}; //!< Polynomial parameters for the uniformity correction for MWall 5"
  std::vector<double> _uniformity_correction_parameters_xwall_{1,1};       //!< Polynomial parameters for the uniformity correction for XWall
  std::vector<double> _uniformity_correction_parameters_gveto_{1,1};       //!< Polynomial parameters for the uniformity correction for GVeto

  // Macro to automate the registration of the module :
  DPP_MODULE_REGISTRATION_INTERFACE(mock_calorimeter_s2c_module)
};

}  // end of namespace processing

}  // end of namespace snemo

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg snemo::processing::mock_calorimeter_s2c_module the name the registered class
DOCD_CLASS_DECLARATION(snemo::processing::mock_calorimeter_s2c_module)

#endif  // FALAISE_SNEMO_PROCESSING_MOCK_CALORIMETER_S2C_MODULE_H

// end of falaise/snemo/processing/mock_calorimeter_s2c_module.h
