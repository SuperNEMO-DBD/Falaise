// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/mock_calorimeter_s2c_module_utils.h
 * Author(s) :    Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2018-04-07
 *
 * License:
 *
 * Description:
 *
 *   Temporary location for 3D polynomial function 
 *   to handle scintillator non-uniformity correction
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_MOCK_CALORIMETER_S2C_MODULE_UTILS_H
#define FALAISE_SNEMO_PROCESSING_MOCK_CALORIMETER_S2C_MODULE_UTILS_H 1

namespace snemo {

  namespace processing {

    double pol3d (const double *x, const double *p);

  }  // end of namespace processing

}  // end of namespace snemo

#endif

// end of falaise/snemo/processing/mock_calorimeter_s2c_module_utils.h
