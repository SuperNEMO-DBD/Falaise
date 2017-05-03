// -*- mode: c++ ; -*-
/* event_header_utils_module.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-01-09
 * Last modified: 2014-10-23
 *
 * License:
 *
 * Description:
 *
 *   Basic module to add info to event header bank
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_EVENT_HEADER_UTILS_MODULE_H
#define FALAISE_SNEMO_PROCESSING_EVENT_HEADER_UTILS_MODULE_H 1

// Third party:
// - Bayeux/dpp:
#include <dpp/base_module.h>

// This project:
#include <falaise/snemo/datamodels/event_header.h>

namespace snemo {

  namespace processing {

    /// \brief A basic module to add informations into the event header bank
    class event_header_utils_module : public dpp::base_module
    {
    public:

      /// Processing mode
      enum mode_type {
        MODE_INVALID    = 0,
        MODE_ADD_HEADER = 0x1
      };

      /// Constructor
      event_header_utils_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~event_header_utils_module();

      /// Initialization
      virtual void initialize(const datatools::properties  & setup_,
                              datatools::service_manager   & service_manager_,
                              dpp::module_handle_dict_type & module_dict_);

      /// Reset
      virtual void reset();

      /// Data record processing
      virtual process_status process(datatools::things & data_);

      /// Check add header mode
      bool is_add_header_mode() const;

      /// Increment run number by one unit
      void ah_increment_run_number(int incr_ = 1);

      /// Increment event number by one unit
      void ah_increment_event_number(int incr_ = 1);

      /// Check real event type
      bool ah_is_real() const;

      /// Check simulated event type
      bool ah_is_simulated() const;

    protected:

      /// Set default values for attributes
      void _set_defaults();

      /// Process for the 'add header' mode
      void _process_add_header(datatools::things & data_record_);

    private:

      // Configuration:
      mode_type   _mode_;                         //!< Processing mode
      std::string _add_header_bank_label_;        //!< Header bank label
      bool        _add_header_update_;            //!< Update the bank itself
      int         _add_header_run_number_;        //!< Run number of data record
      int         _add_header_event_number_;      //!< Event number of data record
      snemo::datamodel::event_header::generation_type _add_header_gentype_; //!< Event generation type
      bool        _add_header_use_genbb_weight_;  //!< Store event weight
      bool        _add_header_use_genbb_label_;   //!< Store event label
      std::string _add_header_properties_path_;   //!< Additional properties to be stored
      std::string _add_header_properties_prefix_; //!< Only store properties
                                                  //!< starting with this prefix

      // Working:
      // For "add header" mode:
      int _ah_current_run_number_;
      int _ah_current_event_number_;

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(event_header_utils_module)

    };

  } // end of namespace processing

} // end of namespace snemo

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg snemo::processing::event_header_utils_module the name the registered class
DOCD_CLASS_DECLARATION(snemo::processing::event_header_utils_module)

#endif // FALAISE_SNEMO_PROCESSING_EVENT_HEADER_UTILS_MODULE_H

// end of event_header_utils_module.h
