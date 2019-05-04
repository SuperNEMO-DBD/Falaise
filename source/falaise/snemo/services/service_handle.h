//! \file service_handle.h
//! \brief Simple access to services via a smart pointer
#ifndef SERVICE_HANDLE_HH
#define SERVICE_HANDLE_HH

#include <exception>

#include "bayeux/datatools/service_manager.h"
#include "falaise/snemo/services/service_traits.h"

namespace snemo {

//! Exception reporting that a service is missing in the provider
class missing_service_error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

//! Exception reporting that a service is provided, but of the wrong type
class bad_service_type : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

//! Exception reporting that a service_handle is empty
class bad_service_access : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

//! Make a @ref service_handle from a @ref datatools::service_manager provider
/*!
 *  Free template function used by constructor of @ref service_handle. As with that
 *  class, the template parameter must have defined @ref service_traits.
 *
 * \tparam T type of service to make
 * \param[in] provider service provider to extract service from
 * \returns raw pointer to service interface
 * \throw missing_service_error if provider cannot supply the service
 * \throw bad_service_type if provider cannot supply a service of type T
 *
 * \sa snemo::service_handle
 * \sa snemo::service_traits
 */
template <typename T>
T* service_maker(datatools::service_manager& provider) {
  using Service_t = typename service_traits<T>::service_type;
  std::string serviceName{service_label<T>::value};

  if (!provider.has(serviceName)) {
    throw missing_service_error{"No service of type '" + serviceName + "' available"};
  }

  if (!provider.is_a<Service_t>(serviceName)) {
    throw bad_service_type{"Service is not of requested type '" + serviceName + "'"};
  }

  // extract and return the underlying type
  return service_traits<T>::get(provider.grab<Service_t>(serviceName));
}

//! \brief Semi-smart pointer holding a service interface
/*!
 * Provides a convenient adaptor interface over @ref datatools::service_manager
 * to ease checked access and use of service interfaces. It may be constructed
 * from a @ref datatools::service_manager instance as
 *
 * ```cpp
 * void example(datatools::service_manager& p) {
 *   snemo::service_handle<some_service_type> someSvc{p};
 *
 *   // ... use someSvc ...
 *   someSvc->some_service_memfn();
 * }
 * ```
 *
 * The template parameter must be a type for which a specialization of @ref
 * snemo::service_traits exists otherwise a compile-time error will be emitted
 * Access and use of the service is checked at construction and access via
 *
 * - The constructor will throw a @ref snemo::missing_service_error or @ref snemo::bad_service_type
 *   if the provider does not hold the service, or it is held but not of the
 *   required type.
 * - The dereference operator will throw @ref snemo::bad_service_access if the
 *   held service interface is `nullptr`
 *
 * It is only a semi-smart pointer as it does not own the raw
 * pointer to the service interface. The lifetime of service interface pointers is expected, but not
 * guaranteed, to be equal to the lifetime of an `flreconstruct` process.
 *
 * \sa falaise::processing::module
 */
template <typename T>
class service_handle {
 public:
  //! Default constructor
  service_handle() = default;

  //! Construct from a service provider
  service_handle(datatools::service_manager& sm) : instance(service_maker<T>(sm)) {}

  //! Return pointer to service interface
  /*!
   * \throws bad_service_access if pointer is null
   */
  T* operator->() const {
    if (instance != nullptr) return instance;

    throw bad_service_access{"service handle is nullptr"};
  }

 private:
  T* instance = nullptr;
};

}  // namespace snemo

#endif  // SERVICE_HANDLE_HH
