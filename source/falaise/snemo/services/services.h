/// \file falaise/snemo/services/services.h
// -*- mode: c++ ; -*-
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-02-27
 * Last modified: 2014-02-27
 *
 * Description:
 *
 *   SuperNEMO service utility
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_SERVICES_H
#define FALAISE_SNEMO_SERVICES_H 1

// Standard library:
#include <string>

namespace snemo {

/// \brief Service infomation
class service_info {
 public:
  /// Return the default string label/name for the 'global metadata' service
  static const std::string& metadataServiceName();

  /// Return the default string label/name for the 'geometry' service
  static const std::string& geometryServiceName();

  /// Return the default string label/name for the 'cuts' service
  static const std::string& cutServiceName();

  /// Return the default string label/name for the 'running conditions' service
  static const std::string& rcServiceName();
};

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_SERVICES_H
