//! \file falaise/snemo/processing/module.h
#ifndef FALAISE_SNEMO_PROCESSING_MODULE_H
#define FALAISE_SNEMO_PROCESSING_MODULE_H

#include <exception>
#include <type_traits>

#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/service_manager.h>
#include <bayeux/dpp/base_module.h>

#include "falaise/config/property_set.h"

namespace falaise {
namespace processing {
//! Enumeration for module to indicate processing success/failure/other
using status = dpp::base_module::process_status;

//! Exception thrown if configuration overwrites reserved keys
class reserved_key_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! \brief A DPP module wrapping a simple processing algorithm
/*!
 * Modules in Falaise's DPP pipeline are concrete classes of @ref dpp::base_module.
 * Authors of modules need to write a significant amount of boilerplate code
 * for the initialize/reset/destruction member functions/states *required* by
 * @ref dpp::base_module. This class template automatically generates these state transitions
 * for the templated type `T`. This type only needs to implement
 * constructors for initialization (via configuration properties) and a `process` member function
 * in which the event data will be processed.
 *
 * The type to be wrapped must meet the requirements:
 *
 *  - `DefaultConstructible`
 *  - `Construtible` with `T(falaise::config::property_set const&, datatools::service_manager&)
 *  - `CopyAssignable`
 *  - Has a member function with signature
 *    ```cpp
 *    falaise::processing::status process(datatools::things& d)
 *    ```
 *
 * For example:
 *
 * ```cpp
 * class MyModule
 * {
 *   MyModule(); // ideally this is =default
 *   MyModule(falaise::config::property_set const& config, datatools::service_manager& services);
 *
 *   falaise::processing::status process(datatools::things& data);
 * };
 * ```
 *
 * The user defined constructor must throw if initialization fails.
 *
 * The class is wrapped and registered with Falaise's DPP plugin system
 * by calling the macro #FALAISE_REGISTER_MODULE(T)
 * in the module's implementation file, e.g.
 *
 * ```cpp
 * // This is MyModule.cpp
 * #include "falaise/snemo/processing/module.h"
 * class MyModule
 * {
 *   ... as above ...
 * };
 *
 * FALAISE_REGISTER_MODULE(MyModule)
 * ```
 *
 * The module will be registered with a string key equal to the typename
 * so that use in pipeline scripts is transparent, e.g.
 *
 * ```ini
 * [name="theLabel" type="MyModule"]
 * ... config ...
 * ```
 *
 * \sa dpp::base_module
 */
template <typename T>
class module : public dpp::base_module {
  static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
  static_assert(
      std::is_constructible<T, falaise::config::property_set const&, datatools::service_manager&>::value,
      "T must have a constructor T(datatools::properties const&, "
      "datatools::services const&)");
  // static_assert(has process member function)
 public:
  //! Destructor
  /*!
   * Moves state to uninitialized as required by dpp::base_module
   */
  virtual ~module() { reset(); }

  //! Change module state to unitialized
  void reset() override { _set_initialized(false); }

  //! Initialize the module
  /*!
   * Constructs an instance of T using its user defined constructor,
   * passing it the module configuration, and services parameters.
   * \param config Parameters passed to configure the module
   * \param services Reference to running service provider
   */
  void initialize(datatools::properties const& config, datatools::service_manager& services,
                  dpp::module_handle_dict_type&) override {
    // Inject required parameters, throwing if config sets these
    if (config.has_key("module_label")) {
      throw reserved_key_error("reserved key 'module_name' passed to module '" + get_name() + "'");
    }
    if (config.has_key("module_type")) {
      throw reserved_key_error("reserved key 'module_type' passed to module '" + get_name() + "'");
    }

    falaise::config::property_set module_config{config};
    module_config.put("module_label", get_name());
    module_config.put("module_type", factory.get_type_id());

    // Assumed to throw if construction is not successful
    wrappedModule = T(module_config, services);

    _set_initialized(true);
  }

  //! Process the input data
  /*!
   *  The data is passed to the process member function of the wrapped type
   *  \param data Reference to the input data
   *  \return An enum reflecting the success/failure/other of the processing
   */
  status process(datatools::things& data) override { return wrappedModule.process(data); };

 private:
  T wrappedModule{};  //! Implementation of the processing algorithm

  // T has to be a friend otherwise it cannot register itself
  friend T;
  using WrapperFactory = ::datatools::_system_factory_registrator<::dpp::base_module, module<T>>;
  static WrapperFactory factory;
};

}  // namespace processing
}  // namespace falaise

//! \def FALAISE_REGISTER_MODULE(T)
/*! Registers T with the module manager
 *  \param T Typename to register
 */
#define FALAISE_REGISTER_MODULE(T)                  \
  namespace falaise {                               \
  namespace processing {                            \
  template <>                                       \
  module<T>::WrapperFactory module<T>::factory{#T}; \
  }                                                 \
  }

#endif  // FALAISE_SNEMO_PROCESSING_MODULE_H
