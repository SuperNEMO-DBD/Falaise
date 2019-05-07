//! \file service_traits.h
//! \brief Types for defining service class traits
#ifndef SERVICE_TRAITS_HH
#define SERVICE_TRAITS_HH

#include "boost/metaparse/string.hpp"
#include "boost/mpl/string.hpp"

namespace snemo {

//! Template struct for defining trait types and functions for a service
/*!
 * No default implementation is replied, so all service classes must fully
 * specialize `service_traits` as follows:
 *
 * ```cpp
 * template <>
 * struct service_traits<MyService> {
 *   // `label_type` is a compile time string, and should be equivalent to the
 *   // service's typename, including any namespace
 *   using label_type = BOOST_METAPARSE_STRING("MyService");
 *
 *   // `service_type` is the type to be extracted from @ref datatools::service_manager
 *   // It is usually *not* the same as the instance type
 *   using service_type = AServiceType;
 *
 *   // `instance_type` is the actual interface type that clients will use via
 *   // @ref falaise::service_handle
 *   using instance_type = MyService;
 *
 *   // `get` is a static function taking a reference to the service_type
 *   // and returning a pointer to the served instance_type.
 *   // In most cases, it calls a member function on the service_type instance
 *   static instance_type* get(service_type& s) {
 *     // get_served returns a ptr to MyService...
 *     return s.get_served();
 *    }
 * };
 * ```
 *
 * \sa snemo::service_handle
 */
template <typename T>
struct service_traits;

//! Convenience type alias for extracting the label of a service
/*!
 * ```cpp
 * // get label for "Service" in string form
 * std::string label = falaise::service_label<Service>::value;
 * ```
 *
 */
template <typename T>
using service_label = typename boost::mpl::c_str<typename service_traits<T>::label_type>::type;

}  // namespace snemo

#endif  // SERVICE_TRAITS_HH
