// -*- mode: c++ ; -*-
/** \file exaegir/resources.h
 * Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2023-09-22
 * Last modified: 2023-09-22
 */

#ifndef EXAEGIR_RESOURCES_H
#define EXAEGIR_RESOURCES_H 1
 
// Standard library:
#include <string>
     
namespace exaegir {

  /// Return the resource installation directory
  std::string get_resource_dir();
  
} // end of namespace exaegir

#endif // EXAEGIR_RESOURCES_H
