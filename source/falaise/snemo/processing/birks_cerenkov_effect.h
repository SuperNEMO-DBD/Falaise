// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/birks_cerenkov_effect.h
 * Author (s) :    F.Mauger <mauger@lpccaen.in2p3.fr>
 *                 E.Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2026-05-18
 * Last modified: 2026-05-18
 * Description:
 *
 *   Model the Birks-Cerenkov correction factor for energy deposit by electrons in scin block.
 *
 */
#ifndef FALAISE_SNEMO_PROCESSING_BIRKS_CERENKOV_EFFECT_H
#define FALAISE_SNEMO_PROCESSING_BIRKS_CERENKOV_EFFECT_H 1

namespace snemo {

  namespace processing {

    struct BirksCerenkovEffect
    {

      static double electron_correction_factor(const double energy_deposit_);
    };
      
  } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_BIRKS_CERENKOV_EFFECT_H
