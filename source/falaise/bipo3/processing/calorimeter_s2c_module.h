// -*- mode: c++ ; -*-
/** \file falaise/bipo3/processing/calorimeter_s2c_module.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-01-12
 * Last modified: 2014-01-30
 *
 * License:
 *
 * Description:
 *
 *   Bipo3 simulation data processor for calorimeter MC hits
 *
 * History:
 *
 */

#ifndef FALAISE_BIPO3_PROCESSING_CALORIMETER_S2C_MODULE_H
#define FALAISE_BIPO3_PROCESSING_CALORIMETER_S2C_MODULE_H 1

// Standard library:
#include <string>
#include <map>
#include <vector>

// Third party:
// - Bayeux/dpp:
#include <dpp/base_module.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/processing/calorimeter_regime.h>

namespace geomtools {
  class manager;
}

namespace mctools {
  class simulated_data;
}

namespace bipo3 {

  namespace processing {

    class calorimeter_s2c_module : public dpp::base_module
    {
    public:

      /// Missing method description
      static void compute_classification (const std::string & particle_name_,
                                          std::string       & classification_);

      /// Collection of calorimeter regime
      typedef std::map<std::string, snemo::processing::calorimeter_regime> calorimeter_regime_col_type;

      /// Collection of geometry category names
      typedef std::vector<std::string>                  category_col_type;

      /// Setting geometry manager
      void set_geom_manager(const geomtools::manager & gmgr_);

      /// Getting geometry manager
      const geomtools::manager & get_geom_manager() const;

      /// Constructor
      calorimeter_s2c_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~calorimeter_s2c_module();

      /// Initialization
      virtual void initialize(const datatools::properties  & setup_,
                              datatools::service_manager   & service_manager_,
                              dpp::module_handle_dict_type & module_dict_);

      /// Reset
      virtual void reset();

      /// Data record processing
      virtual process_status process(datatools::things & data_);

    protected:

      /// Getting random number generator
      mygsl::rng & _get_random ();

      /// Digitize calorimeter hits
      void _process_calorimeter_digitization
      (const mctools::simulated_data & simulated_data_,
       snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_);

      /// Calibrate calorimeter hits (energy/time resolution spread)
      void _process_calorimeter_calibration
      (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calorimeter_hits_);

      /// Apply basic trigger effect
      void _process_calorimeter_trigger
      (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calorimeter_hits_);

      /// Main process function
      void _process
      (const mctools::simulated_data & simulated_data_,
       snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_);

    private:

      const geomtools::manager * _geom_manager_; //!< Handle to an external geometry manager

      mygsl::rng _random_; //!< Embedded PRNG

      category_col_type           _hit_categories_;       //!< Calorimeter categories
      calorimeter_regime_col_type _calorimeter_regimes_;  //!< Calorimeter regime tools

      std::string _SD_label_;  //!< The label of the simulated data bank
      std::string _CD_label_;  //!< The label of the calibrated data bank

      double _cluster_time_width_; //!< Time width of a calo cluster
      bool   _alpha_quenching_;    //!< Flag to (dis)activate the alpha quenching

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(calorimeter_s2c_module)

    };

  } // end of namespace processing

} // end of namespace bipo3

#endif // FALAISE_BIPO3_PROCESSING_CALORIMETER_S2C_MODULE_H
