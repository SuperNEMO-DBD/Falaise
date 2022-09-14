//! \file falaise/snemo/processing/filter.h
#ifndef FALAISE_SNEMO_PROCESSING_FILTER_H
#define FALAISE_SNEMO_PROCESSING_FILTER_H

#include <stdexcept>
#include <type_traits>

#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/service_manager.h>
#include <bayeux/dpp/base_module.h>

#include "falaise/property_set.h"

#include "falaise/snemo/processing/types.h"

namespace falaise {
namespace processing {

//! \brief A DPP module wrapping a simple filtering algorithm
/*!
 * An extension of @ref falaise::processing::module to implement filtering of events.
 *
 * Whereas @ref falaise::processing::module is intended to wrap algorithms that read and
 * write data to events ("producer"), `filter` wraps algorithms that only read the data
 * in the event and make an if/else decision on how to proceed with further processing.
 * Each branch of this decision forwards the event to a module/filter to handle onward
 * processing.
 *
 * This class template templates the handling of the input and if/else output modules
 * for a type `T` that implements the `filter` concept. This type only needs to implement
 * constructors for initialization (via configuration properties) and a `filter` member function
 * returning a boolean indicating the if/else decision.
 *
 * The type to be wrapped must meet the requirements:
 *
 *  - `DefaultConstructible`
 *  - `Construtible` with `T(falaise::property_set const&, datatools::service_manager&)
 *  - `CopyAssignable`
 *  - Has a member function with signature
 *    ```cpp
 *    bool filter(const datatools::things& d)
 *    ```
 *
 * For example:
 *
 * ```cpp
 * class MyFilter
 * {
 *   MyFilter(); // ideally this is =default
 *   MyFilter(falaise::property_set const& config, datatools::service_manager& services);
 *
 *   bool process(const datatools::things& data);
 * };
 * ```
 *
 * The user defined constructor must throw if initialization fails.
 *
 * The class is wrapped and registered with Falaise's pipeline plugin system
 * by calling the macro #FALAISE_REGISTER_FILTER(T)
 * in the filters's implementation file, e.g.
 *
 * ```cpp
 * // This is MyFilter.cpp
 * #include "falaise/snemo/processing/filter.h"
 * class MyFilter
 * {
 *   ... as above ...
 * };
 *
 * FALAISE_REGISTER_FILTER(MyFilter)
 * ```
 *
 * The module will be registered with a string key equal to the typename
 * so that use as a module in pipeline scripts is transparent, e.g.
 *
 * ```ini
 * [name="theLabel" type="MyFilter"]
 * ... config ...
 * ```
 *
 * The filter `process` member function must return `true` if the supplied
 * event passes the filter, and `false` otherwise. Onward processing of the
 * event can be selected through the module level `on_accept` and `on_reject`
 * string parameters:
 *
 * ```ini
 * [name="theLabel", type="MyFilter"]
 * on_accept : string = "A"
 * on_reject : string = "B"
 * ... filter config ...
 *
 * [name="A", type="AlgoA"]
 * ...
 *
 * [name="B", type="AlgoB"]
 * ...
 * ```
 *
 * where these parameters must be valid names of other modules in the
 * configuration. There is no limit on the type of module here, so chains
 * of processing and filtering modules can be built up. Note however that
 * whilst recursion/iteration is permitted, it is not yet recommended and may lead
 * to infinite loops.
 *
 * \sa falaise::processing::module
 * \sa dpp::base_module
 */
template <typename T>
class filter : public dpp::base_module {
  static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
  static_assert(
      std::is_constructible<T, falaise::property_set const&, datatools::service_manager&>::value,
      "T must have a constructor T(datatools::properties const&, "
      "datatools::services const&)");
  // static_assert(has filter member function)
 public:
  //! Destructor
  /*!
   * Moves state to uninitialized as required by dpp::base_module
   */
  virtual ~filter() { reset(); }

  //! Change module state to unitialized
  void reset() override { _set_initialized(false); }

  //! Initialize the module
  /*!
   * Constructs an instance of T using its user defined constructor,
   * passing it the module configuration, services, and module manager parameters.
   * \param config Parameters passed to configure the module
   * \param services Reference to running service provider
   * \param modules Reference to map of modules available to this configuration
   */
  void initialize(datatools::properties const& config, datatools::service_manager& services,
                  dpp::module_handle_dict_type& modules) override {
    // Inject/extract required parameters, throwing if config sets these
    if (config.has_key("module_label")) {
      throw reserved_key_error("reserved key 'module_name' passed to module '" + get_name() + "'");
    }
    if (config.has_key("module_type")) {
      throw reserved_key_error("reserved key 'module_type' passed to module '" + get_name() + "'");
    }

    falaise::property_set module_config{config};
    module_config.put("module_label", get_name());
    module_config.put("module_type", factory.get_type_id());

    try {
      const std::string passBranchName = module_config.get<std::string>("on_accept");
      module_config.erase("on_accept");
      auto passIter = modules.find(passBranchName);
      DT_THROW_IF(passIter == modules.end(), missing_module_error,
                  "No module '" << passBranchName << "' found in configuration");
      passBranch_ = (passIter->second).grab_initialized_module_handle();

      const std::string failBranchName = module_config.get<std::string>("on_reject");
      module_config.erase("on_reject");
      auto failIter = modules.find(failBranchName);
      DT_THROW_IF(failIter == modules.end(), missing_module_error,
                  "No module '" << failBranchName << "' found in configuration");
      failBranch_ = (failIter->second).grab_initialized_module_handle();

      wrappedFilter_ = T(module_config, services);

    } catch (const falaise::missing_key_error& e) {
      std::ostringstream oss{};
      oss << "initialization of filter '" << get_name() << "' (type '" << factory.get_type_id()
          << "') failed with exception:\n"
          << "- missing_key_error: " << e.what() << "\n";
      config.tree_dump(oss, "- config:");
      throw configuration_error{oss.str()};
    } catch (const falaise::wrong_type_error& e) {
      std::ostringstream oss{};
      oss << "initialization of filter '" << get_name() << "' (type '" << factory.get_type_id()
          << "') failed with exception:\n"
          << "- wrong_type_error: " << e.what() << "\n";
      config.tree_dump(oss, "- config:");
      throw configuration_error{oss.str()};
    } catch (const missing_module_error& e) {
      std::ostringstream oss{};
      oss << "initialization of filter '" << get_name() << "' (type '" << factory.get_type_id()
          << "') failed with exception:\n"
          << "- missing_module_error: " << e.what() << "\n";
      config.tree_dump(oss, "- config:");
      throw configuration_error{oss.str()};
    } catch (const std::exception& e) {
      std::ostringstream oss{};
      oss << "initialization of filter '" << get_name() << "' (type '" << factory.get_type_id()
          << "') failed with exception:\n"
          << "- <unknown>: " << e.what() << "\n";
      config.tree_dump(oss, "- config:");
      throw configuration_error{oss.str()};
    }

    _set_initialized(true);
  }

  //! Filter the input data
  /*!
   *  The data is passed to the `filter` member function of the wrapped type
   *  and forwarded to the appropriate branch based on the filter's decision.
   *
   *  \param data Reference to the input data
   *  \return An enum reflecting the success/failure/other of the processing
   */
  status process(datatools::things& data) override {
    if (wrappedFilter_.filter(data)) {
      return passBranch_->process(data);
    }
    return failBranch_->process(data);
  };

 private:
  T wrappedFilter_{};  //! Implementation of the filter algorithm

  using module_ptr = dpp::module_handle_type;
  module_ptr passBranch_;  //! module to forward data to if filter succeeds
  module_ptr failBranch_;  //! module to forward data to if filter fails;

  // T has to be a friend otherwise it cannot register itself
  friend T;
  using WrapperFactory = ::datatools::_system_factory_registrator<::dpp::base_module, filter<T>>;
  static WrapperFactory factory;
};

}  // namespace processing
}  // namespace falaise

//! \def FALAISE_REGISTER_FILTER(T)
/*! Registers T with the module manager
 *  \param T Typename to register
 */
#define FALAISE_REGISTER_FILTER(T)                  \
  namespace falaise {                               \
  namespace processing {                            \
  template <>                                       \
  filter<T>::WrapperFactory filter<T>::factory{#T}; \
  }                                                 \
  }

#endif  // FALAISE_SNEMO_PROCESSING_FILTER_H