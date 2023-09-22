// -*- mode: c++ ; -*-
/** \file exaegir/version.h
 * Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2023-09-22
 * Last modified: 2023-09-22
 */

#ifndef EXAEGIR_VERSION_H
#define EXAEGIR_VERSION_H 1
 
// Standard library:
#include <string>
     
namespace exaegir {

  /// Return the library version string identifier
  std::string version_string();
  
} // end of namespace exaegir

#endif // EXAEGIR_VERSION_H
