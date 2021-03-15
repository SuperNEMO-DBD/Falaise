/// \file snsim/em_field_g4_utils.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-04-20
 * Last modified: 2015-04-20
 *
 * License:
 *
 * Description:
 *
 *   G4 electromagnetic field utilities
 *
 * History:
 *
 */

#ifndef SNSIM_EM_FIELD_G4_UTILS_H
#define SNSIM_EM_FIELD_G4_UTILS_H

namespace snsim {

enum position_time_index {
  POSTIME_X = 0,  //!< Index of the X coordinate
  POSTIME_Y = 1,  //!< Index of the Y coordinate
  POSTIME_Z = 2,  //!< Index of the Z coordinate
  POSTIME_T = 3   //!< Index of the time coordinate
};

enum emfield_index {
  EMFIELD_BX = 0,  //!< Index of the Bx coordinate
  EMFIELD_BY = 1,  //!< Index of the By coordinate
  EMFIELD_BZ = 2,  //!< Index of the Bz coordinate
  EMFIELD_EX = 3,  //!< Index of the Ex coordinate
  EMFIELD_EY = 4,  //!< Index of the Ey coordinate
  EMFIELD_EZ = 5   //!< Index of the Ez coordinate
};

}  // namespace snsim

#endif  // SNSIM_EM_FIELD_G4_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
