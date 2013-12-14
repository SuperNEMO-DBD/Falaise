// -*- mode: c++; -*-
/// \file foo_module.h

#ifndef FALAISE_EXAMPLE_FOO_MODULE_H
#define FALAISE_EXAMPLE_FOO_MODULE_H

// Standard library
#include <string>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h> // Logging utility
// - Bayeux/dpp
#include <dpp/base_module.h> // The parent module class

namespace falaise {
namespace example {

  /// \brief A dummy module that adds a bank data in the event record.
  /// The added bank is of type "datatools::properties". A flag with arbitrary name
  /// is stored in the 'properties' object.
  class foo_module : public dpp::base_module
  {
  public:

    /// Return default key/name of the *foo* bank :
    static const std::string & default_foo_bank_key();

    /// Return default key/name of the *foo* flag :
    static const std::string & default_foo_flag_key();

    //   ******************************
    //   * Mandatory module interface *
    //   ******************************

    /// Constructor
    foo_module(datatools::logger::priority logging_priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~foo_module();

    /// Initialization method
    virtual void initialize(const datatools::properties & a_config,
                            datatools::service_manager & a_service_manager,
                            dpp::module_handle_dict_type & a_module_dictionnary);

    /// Termination method
    virtual void reset();

    /// Main data record processing method
    virtual dpp::base_module::process_status process(datatools::things & a_data_record);

    //   **********************
    //   * Specific interface *
    //   **********************

    /// Set the name of the bank where to store the object
    void set_foo_bank_key(const std::string &);

    /// Get the name of the bank where to store the object
    const std::string & get_foo_bank_key() const;

    /// Set the name of the flag set in the object
    void set_foo_flag_key(const std::string &);

    /// Get the name of the flag set in the object
    const std::string & get_foo_flag_key() const;

  private:

    std::string _foo_bank_key_; /** The key/name of the data bank where
                                   * the generated object must be stored
                                   * in the data record.
                                   */
    std::string _foo_flag_key_; //!< The key/name of the 'flag' property (boolean) to be stored in the

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(foo_module);

  };

} // end of namespace example
} // end of namespace falaise

// Object configuration description (OCD) support :
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(falaise::example::foo_module)

#endif // FALAISE_EXAMPLE_FOO_MODULE_H
