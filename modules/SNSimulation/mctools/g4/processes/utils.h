/// \file mctools/g4/processes/em_utils.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-09-03
 * Last modified: 2015-09-04
 *
 * License:
 *
 * Description:
 *
 *   G4 electromagnetic model factory
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_PROCESSES_UTILS_H
#define MCTOOLS_G4_PROCESSES_UTILS_H

// Standard Library:
#include <string>

// Third party:
// - Geant4:
#include <G4ProcessType.hh>

namespace mctools {

namespace g4 {

namespace processes {

std::string process_type_to_label(G4ProcessType);

G4ProcessType label_to_process_type(const std::string& label_);

std::string em_process_subtype_to_label(int);

int label_to_em_process_subtype(const std::string& label_);

}  // end of namespace processes

}  // end of namespace g4

}  // end of namespace mctools

#endif  // MCTOOLS_G4_PROCESSES_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
