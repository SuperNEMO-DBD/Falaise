// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/mock_tracker_s2c_module.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-01-09
 * Last modified: 2014-02-27
 *
 * License:
 *
 * Description:
 *
 *   Mock simulation data processor for tracker MC hits
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_MOCK_TRACKER_S2C_MODULE_H
#define FALAISE_SNEMO_PROCESSING_MOCK_TRACKER_S2C_MODULE_H 1

// Standard library:
#include <map>
#include <string>
#include <vector>

// Third party:
#include <bayeux/dpp/base_module.h>
#include <bayeux/mctools/simulated_data.h>
#include <bayeux/mygsl/rng.h>

// This project :
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/processing/geiger_regime.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/snemo/services/service_handle.h>

namespace geomtools {
class manager;
}

namespace snreco {
namespace detail {
class mock_raw_tracker_hit;
}
}  // namespace snreco

namespace snemo {

namespace processing {

/// \brief Simple modelling of the time and space measurement with the SuperNEMO drift cells in
/// Geiger mode
class mock_tracker_s2c_module : public dpp::base_module {
 public:
  // Because dpp::base_module is insane
  virtual ~mock_tracker_s2c_module() { this->reset(); }

  /// Initialization
  virtual void initialize(datatools::properties const& ps, datatools::service_manager& services,
                          dpp::module_handle_dict_type&) override;

  /// Reset
  virtual void reset() override;

  /// Data record processing
  virtual process_status process(datatools::things& event) override;

 private:
  // Rationalized typenames
  using sim_tracker_hit_col_t = mctools::simulated_data::hit_handle_collection_type;
  using raw_tracker_hit_col_t = std::list<snreco::detail::mock_raw_tracker_hit>;
  using cal_tracker_hit_col_t = snemo::datamodel::calibrated_data::tracker_hit_collection_type;

  /// Digitize tracker hits
  raw_tracker_hit_col_t digitizeHits_(const sim_tracker_hit_col_t& steps);

  /// Calibrate tracker hits (longitudinal and transverse spread)
  cal_tracker_hit_col_t calibrateHits_(const raw_tracker_hit_col_t& digits);

  /// Main process function
  cal_tracker_hit_col_t process_(const sim_tracker_hit_col_t& hits);

  snemo::service_handle<snemo::geometry_svc> geoManager{};  //!< The geometry manager
  std::string _module_category_{};  //!< The geometry category of the SuperNEMO module
  std::string _hit_category_{};     //!< The category of the input Geiger hits
  geiger_regime _geiger_{};         //!< Geiger regime tools
  mygsl::rng RNG_{};                //!< internal PRN generator
  double _peripheral_drift_time_threshold_{
      datatools::invalid_real_double()};  //!< Peripheral drift time threshold
  double _delayed_drift_time_threshold_{
      datatools::invalid_real_double()};   //!< Delayed drift time threshold
  std::string sdInputTag{};                //!< The label of the simulated data bank
  std::string cdOutputTag{};               //!< The label of the calibrated data bank
  bool _store_mc_hit_id_{false};           //!< Flag to store the MC true hit ID
  bool _store_mc_truth_track_ids_{false};  //!< The flag to reference the MC engine track and parent
                                           //!< track IDs associated to this calibrated Geiger hit

  // Macro to automate the registration of the module :
  DPP_MODULE_REGISTRATION_INTERFACE(mock_tracker_s2c_module)
};

}  // end of namespace processing

}  // end of namespace snemo

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg snemo::processing::mock_tracker_s2c_module the name the registered class
DOCD_CLASS_DECLARATION(snemo::processing::mock_tracker_s2c_module)

#endif  // FALAISE_SNEMO_PROCESSING_MOCK_TRACKER_S2C_MODULE_H

// end of falaise/snemo/processing/mock_tracker_s2c_module.h
