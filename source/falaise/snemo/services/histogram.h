#ifndef HISTOGRAM_HH
#define HISTOGRAM_HH


#include "bayeux/dpp/histogram_service.h"

namespace snemo {
using histogram = dpp::histogram_service;

template <>
struct service_traits<histogram> {
  using label_type = BOOST_METAPARSE_STRING("histogram");
  using service_type = histogram;
  using instance_type = histogram;

  static instance_type* get(service_type& sm) {
    return &(sm);
  }
};

} // namespace snemo

#endif // HISTOGRAM_HH

