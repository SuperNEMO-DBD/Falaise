// -*- mode: c++ ; -*-
/** \file fecom/src/libs/libfecom/fecom/mock_hc2cd_module.hpp
 * Author(s) :    Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>
 * Creation date: 2017-04-04
 * Last modified: 2017-04-04
 *
 * License:
 *
 * Description:
 *
 *   Mock simulation data processor for raw half commissioning events
 *
 * History:
 *
 */

#ifndef FECOM_MOCK_HC2CD_MODULE_HPP
#define FECOM_MOCK_HC2CD_MODULE_HPP 1

// Standard library:
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <fstream>

// Third party:
// - Bayeux/dpp:
#include <dpp/base_module.h>

// - Falaise:
#include <falaise/snemo/datamodels/mock_raw_tracker_hit.h>
#include <falaise/snemo/processing/geiger_regime.h>
#include <falaise/snemo/datamodels/calibrated_data.h>

// This project :
#include <fecom/commissioning_event.hpp>

namespace geomtools {
  class manager;
}

namespace fecom {

  /// \brief Simple calibration of raw half commissioning events
  class mock_hc2cd_module
    : public dpp::base_module
  {
  public:

    struct data_info
    {
      /// \deprecated Default string label/name for the 'simulated data'
      static const std::string HALF_COM_RAW_DATA_LABEL;

      /// default string label/name for the 'half commissioning raw data'
      static const std::string & default_half_com_raw_data_label();
    };

    /// Constructor
    mock_hc2cd_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~mock_hc2cd_module();

    /// Set the channel mapping
    void set_channel_mapping(const fecom::channel_mapping & chmap_);

    /// Getting geometry manager
    const fecom::channel_mapping & get_channel_mapping() const;

    /// Set the geometry manager
    void set_geom_manager(const geomtools::manager & gmgr_);

    /// Getting geometry manager
    const geomtools::manager & get_geom_manager() const;

    /// GID to falaise GID convertor
    void convert_gid_to_falaise_gid(const geomtools::geom_id geometric_id_,
				    geomtools::geom_id & falaise_geom_id_);

    /// Initialization
    virtual void initialize(const datatools::properties  & setup_,
			    datatools::service_manager   & service_manager_,
			    dpp::module_handle_dict_type & module_dict_);

    /// Reset
    virtual void reset();

    /// Data record processing
    virtual process_status process(datatools::things & event_record_);

  protected:

    /// Set default attributes values
    void _set_defaults();

    /// Calibrate calo hits
    void _process_calo_calibration(const fecom::commissioning_event & hc_raw_com_event_,
				   snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calo_hits_);

    /// Calibrate tracker hits (longitudinal and transverse spread)
    void _process_tracker_calibration(const fecom::commissioning_event & hc_raw_com_event_,
				      snemo::datamodel::calibrated_data::tracker_hit_collection_type & calibrated_tracker_hits_);

    /// Main process function
    void _process(const fecom::commissioning_event & hc_raw_data_,
		  snemo::datamodel::calibrated_data & the_calibrated_data_);

  private:

    const fecom::channel_mapping * _channel_mapping_; //!< The channel mapping
    const geomtools::manager * _geom_manager_;        //!< The geometry manager
    std::string   _module_category_;                  //!< The geometry category of the SuperNEMO module
    snemo::processing::geiger_regime _geiger_;        //!< Geiger regime tools
    std::string   _HCRD_label_;                       //!< The label of the half commissioning raw data bank
    std::string   _CD_label_;                         //!< The label of the calibrated data bank
    std::string   _Geo_label_;                        //!< The label of the geometry service
    double _tracker_clock_tick_;
    std::unique_ptr<std::ofstream> _fout_;


    // Macro to automate the registration of the module :
    // DPP_MODULE_REGISTRATION_INTERFACE(mock_hc2cd_module)

  };

} // end of namespace fecom

// /***************************
//  * OCD support : interface *
//  ***************************/

// #include <datatools/ocd_macros.h>

// // @arg fecom::mock_hc2cd_module the name the registered class
// DOCD_CLASS_DECLARATION(fecom::mock_hc2cd_module)

#endif // FECOM_MOCK_HC2CD_MODULE_HPP

// end of fecom/src/libs/libfecom/fecom/mock_hc2cd_module.hpp
