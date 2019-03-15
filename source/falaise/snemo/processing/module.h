//! \file falaise/snemo/processing/module.h
#ifndef FALAISE_SNEMO_PROCESSING_MODULE_H
#define FALAISE_SNEMO_PROCESSING_MODULE_H

#include <type_traits>

#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/service_manager.h>
#include <bayeux/dpp/base_module.h>

namespace falaise {
namespace processing {
//! Enumeration for module to indicate processing success/failure/other
using status = dpp::base_module::process_status;

//! \brief A DPP module that wraps a simple processing algorithm
/*!
 * Modules in Falaise's DPP pipeline are concrete classes of dpp::base_module.
 * Authors of modules need to write a significant amount of boilerplate code
 * for the initialize/reset/destruction member functions/states *required* by
 * DPP. This class template automatically generates these state transitions
 * for the templated type `T`. This type then only needs to implement
 * constructors and the data processing member function.
 *
 * The type to be wrapped must meet the requirements:
 *
 *  - `DefaultConstructible`
 *  - `Construtible` with `T(std::string const&, datatools::properties const&,
 * datatools::service_manager&)
 *  - `CopyAssignable`
 *  - Has a member function with signature
 *    \code{.cpp}
 *    falaise::processing::status
 *    process(datatools::things& d)
 *    \endcode
 *
 * For example:
 *
 * \code{.cpp}
 * class MyModule
 * {
 *   MyModule(); // ideally this is =default
 *   MyModule(std::string const& name, datatools::properties const& config,
 * datatools::service_manager& services);
 *
 *   falaise::processing::status
 *   process(datatools::things& data);
 * };
 * \endcode
 *
 * The user defined constructor should throw if initialization fails.
 *
 * The class is wrapped and registered with Falaise's DPP plugin system
 * by calling the macro #FALAISE_REGISTER_MODULE(T, Key)
 * in the module's implementation file, e.g.
 *
 * \code{.cpp}
 * // This is MyModule.cpp
 * #include "falaise/snemo/processing/module.h"
 * class MyModule
 * {
 *   ... as above ...
 * };
 *
 * FALAISE_REGISTER_MODULE(MyModule, "MyModule")
 *
 * \endcode
 *
 * It is strongly recommended to set the `Key` string to the typename
 * so that use in pipeline scripts is transparent.
 *
 * \sa dpp::base_module
 */
template <typename T>
class module : public dpp::base_module {
  static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
  static_assert(std::is_constructible<T, std::string const&, datatools::properties const&,
                                      datatools::service_manager&>::value,
                "T must have a constructor T(std::string const&, datatools::properties const&, "
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
   * passing it the module name, config, and services parameters.
   * \param config Parameters passed to configure the module
   * \param services Reference to running service provider
   */
  void initialize(datatools::properties const& config, datatools::service_manager& services,
                  dpp::module_handle_dict_type&) override {
    wrappedModule = T(get_name(), config, services);
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

//! \def FALAISE_REGISTER_MODULE(T, Key)
/*! Registers T with the module manager referenced by the given key
 *  \param T Typename register
 *  \param Key String ID with which to register the module
 */
#define FALAISE_REGISTER_MODULE(T, Key)              \
  namespace falaise {                                \
  namespace processing {                             \
  template <>                                        \
  module<T>::WrapperFactory module<T>::factory{Key}; \
  }                                                  \
  }

#endif  // FALAISE_SNEMO_PROCESSING_MODULE_H
