// falaise/snemo/processing/config.h
/* Author(s) :    F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-12-05
 * Last modified: 2022-12-05
 *
 * License: GPL3
 *
 * Description:
 *
 *   Reconstruction configuration utilities for SuperNEMO.
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_CONFIG_H
#define FALAISE_SNEMO_PROCESSING_CONFIG_H 1

// Standard library:
#include <string>

namespace snemo {

  namespace processing {

    const std::string & default_reconstruction_tag();
    
  } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_CONFIG_H
