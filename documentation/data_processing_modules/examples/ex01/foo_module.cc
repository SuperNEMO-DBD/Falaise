// Ourselves
#include "foo_module.h"

// Third party
// - Bayeux/datatools
#include <datatools/exception.h> // Exception utility

namespace falaise {
namespace example {

  // Registration instantiation macro
  // The "falaise::example::foo_module" must be unique.
  DPP_MODULE_REGISTRATION_IMPLEMENT(foo_module, "falaise::example::foo_module");

  // Static
  const std::string & foo_module::default_foo_bank_key()
  {
    static const std::string key("Foo");
    return key;
  }

  // Static
  const std::string & foo_module::default_foo_flag_key()
  {
    static const std::string key("Test");
    return key;
  }

  // Constructor:
  foo_module::foo_module(datatools::logger::priority logging_priority)
    : dpp::base_module(logging_priority)
  {
  }

  foo_module::~foo_module()
  {
    // Make sure all internal resources are terminated before destruction :
    if (is_initialized()) reset();
    return;
  }

  void foo_module::initialize(const datatools::properties & a_config,
                              datatools::service_manager & a_service_manager,
                              dpp::module_handle_dict_type & a_module_dictionnary)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Foo module ``" << get_name() << "`` is already initialized !");

    // Parsing configuration properties:
    _common_initialize(a_config);

    if (_foo_bank_key_.empty()) {
      // Optional ``foo_bank_key`` property :
      if (a_config.has_key("foo_bank_key")) {
        std::string foo_bank_key = a_config.fetch_string("foo_bank_key");
        set_foo_bank_key(foo_bank_key);
      }
    }

    if (_foo_flag_key_.empty()) {
      // Optional ``flag_key`` property :
      if (a_config.has_key("foo_flag_key")) {
        std::string flag_key = a_config.fetch_string("foo_flag_key");
        set_foo_flag_key(flag_key);
      }
    }

    // Initialization:
    if (_foo_bank_key_.empty()) {
      _foo_bank_key_ = foo_module::default_foo_bank_key();
    }
    if (_foo_flag_key_.empty()) {
      _foo_flag_key_ = foo_module::default_foo_flag_key();
    }

    _set_initialized(true);
    return;
  }


  void foo_module::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Foo module '" << get_name() << "' is not initialized !");
    _set_initialized(false);
    _foo_bank_key_.clear();
    _foo_flag_key_.clear();
    return;
  }


  dpp::base_module::process_status
  foo_module::process(datatools::things & a_data_record)
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Foo module '" << get_name() << "' is not initialized !");

    // Check:
    if (a_data_record.has(_foo_bank_key_)) {
      DT_THROW_IF(a_data_record.is_a<datatools::properties>(_foo_bank_key_),
                  std::logic_error,
                  "Existing bank named '" << _foo_bank_key_ << "' with incompatible type !");
      a_data_record.erase(_foo_bank_key_);
    }

    // Add a new bank of type ``datatools::properties`` with requested nam/keye :
    datatools::properties & aFoo = a_data_record.add<datatools::properties>(_foo_bank_key_);

    // Now store a boolean flag with given name/key in the property container addressed
    // by the 'Foo' bank:
    aFoo.store_flag(_foo_flag_key_);

    return dpp::base_module::PROCESS_OK;
  }

  void foo_module::set_foo_bank_key(const std::string & key_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Foo module ``" << get_name() << "`` is already initialized !");
    _foo_bank_key_ = key_;
    return;
  }

  const std::string & foo_module::get_foo_bank_key() const
  {
    return _foo_bank_key_;
  }

  void foo_module::set_foo_flag_key(const std::string & key_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Foo module ``" << get_name() << "`` is already initialized !");
    _foo_flag_key_ = key_;
    return;
  }

  const std::string & foo_module::get_foo_flag_key() const
  {
    return _foo_flag_key_;
  }

} // end of namespace example
} // end of namespace falaise
