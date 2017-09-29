/// \file falaise/snemo/processing/services.h
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

#ifndef FALAISE_SNEMO_PROCESSING_SERVICES_H
#define FALAISE_SNEMO_PROCESSING_SERVICES_H 1

// Standard library:
#include <string>

namespace snemo {

namespace processing {

/// \brief Service utility
class service_info {
 public:
  /// Return the default string label/name for the 'global metadata' service
  static const std::string& default_metadata_service_label();

  /// Return the default string label/name for the 'geometry' service
  static const std::string& default_geometry_service_label();

  /// Return the default string label/name for the 'electronics' service
  static const std::string& default_electronics_service_label();

  /// Return the default string label/name for the 'cuts' service
  static const std::string& default_cut_service_label();

  /// Return the default string label/name for the 'database' service
  static const std::string& default_database_service_label();
};

}  // end of namespace processing

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_PROCESSING_SERVICES_H
