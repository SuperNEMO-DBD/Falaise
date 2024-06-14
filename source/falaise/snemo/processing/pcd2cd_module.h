// -*- mode: c++ ; -*-
/// \file falaise/snemo/processing/pcd2cd_module.h
/* Author(s) :    Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2023-09-21
 * Last modified: 2023-09-21
 *
 * Description:
 *
 *   Precalibrated data to calibrated data processing module
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_PCD2CD_MODULE_H
#define FALAISE_SNEMO_PROCESSING_PCD2CD_MODULE_H 1

// Standard library:
#include <map>
#include <string>
#include <vector>

// Third party:
// - Bayeux/dpp:
#include <dpp/base_module.h>
// - CLHEP
#include <CLHEP/Units/SystemOfUnits.h>

// This project :
#include <falaise/snemo/datamodels/precalibrated_data.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/processing/module.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/snemo/services/service_handle.h>

namespace geomtools {
  class manager;
}


namespace snemo {

  namespace processing {

    /// \brief A processing module for pCD data to pCD tracker and calorimeter hits
    class pcd2cd_module : public dpp::base_module {

      enum calorimeter_energy_calibration_method {
	CALO_ENERGY_NONE           = 0,
	CALO_ENERGY_200MV          = 1,
	CALO_ENERGY_POL0_TABLE     = 2,
	CALO_ENERGY_POL1_TABLE     = 3
      };

      enum calorimeter_time_calibration_method {
	CALO_TIME_NONE             = 0,
	CALO_TIME_T0_TABLE         = 1
      };

      enum tracker_time_calibration_method {
	TRACKER_TIME_NONE          = 0,
	TRACKER_TIME_T0_TABLE      = 1
      };

      enum tracker_radius_calibration_method {
	TRACKER_RADIUS_NONE        = 0,
	TRACKER_RADIUS_FALAISE     = 1,
	TRACKER_RADIUS_MANU        = 2
      };

      enum tracker_height_calibration_method {
	TRACKER_HEIGHT_NONE        = 0,
	TRACKER_HEIGHT_LINEAR_R5R6 = 1
      };

    public:
      // Because dpp::base_module is insane
      virtual ~pcd2cd_module() { this->reset(); }

      /// Initialization
      virtual void initialize(const datatools::properties& ps, datatools::service_manager& /*unused*/,
                              dpp::module_handle_dict_type& /*unused*/);

      /// Reset
      virtual void reset();

      /// Data record processing
      virtual process_status process(datatools::things& event);

    private:

      /// Parse calibration constants from a database file
      int parse_calibration_constants (std::string path_, std::vector<std::vector<double>> & constants_);

      /// Calibrate calorimeter hit
      bool calibrate_calo_hit(const snemo::datamodel::precalibrated_calorimeter_hit & pcd_calo_hit_,
			      snemo::datamodel::calibrated_calorimeter_hit & cd_calo_hit_);

      /// Main process calo function
      void process_calo_impl(const snemo::datamodel::precalibrated_data & pcd_data_,
                             snemo::datamodel::calibrated_data & cd_data_);

      /// Calibrate tracker hit
      bool calibrate_tracker_hit(const snemo::datamodel::precalibrated_tracker_hit & pcd_tracker_hit_,
				 snemo::datamodel::calibrated_tracker_hit & cd_tracker_hit_);

      /// Main process tracker function
      void process_tracker_impl(const snemo::datamodel::precalibrated_data & pcd_data_,
                             snemo::datamodel::calibrated_data & cd_data_);

    private:
      snemo::service_handle<snemo::geometry_svc> geoManager{};  //!< The geometry manager

      std::string _pcd_input_tag_{};   //!< The label of the precalibrated data bank
      std::string _cpcd_input_tag_{}; //!< The label of the clusterized precalibrated data bank

      std::string _cd_output_tag_{};   //!< The label of the calibrated data bank
      std::string _ccd_output_tag_{};  //!< The label of the clusterized calibrated data bank

      // double _event_time_;

      calorimeter_energy_calibration_method _pcd2cd_calo_energy_method_;
      std::vector<std::vector<double>> _pcd_calo_energy_constants_;

      calorimeter_time_calibration_method   _pcd2cd_calo_time_method_;
      std::vector<std::vector<double>> _pcd_calo_t0_constants_;

      tracker_time_calibration_method _pcd2cd_tracker_time_method_;
      std::vector<std::vector<double>> _pcd_tracker_anode_t0_constants_;
      std::vector<std::vector<double>> _pcd_tracker_bottom_cathode_t0_constants_;
      std::vector<std::vector<double>> _pcd_tracker_top_cathode_t0_constants_;

      tracker_radius_calibration_method _pcd2cd_tracker_radius_method_;
      tracker_height_calibration_method _pcd2cd_tracker_height_method_;

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(pcd2cd_module)

    };

  }  // end of namespace processing

}  // end of namespace snemo

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg snemo::processing::pcd2cd_module the name the registered class
DOCD_CLASS_DECLARATION(snemo::processing::pcd2cd_module)

#endif  // FALAISE_SNEMO_PROCESSING_PCD2CD_MODULE_H

// end of falaise/snemo/processing/pcd2cd_module.h
