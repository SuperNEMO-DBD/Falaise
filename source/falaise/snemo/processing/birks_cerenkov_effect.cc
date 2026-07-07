
// Ourselves:
#include "birks_cerenkov_effect.h"

// Standard library:
#include <cmath>

#include <bayeux/datatools/clhep_units.h>

namespace snemo {

  namespace processing {

    // static
    double BirksCerenkovEffect::electron_correction_factor(const double energy_deposit_)
    {
      double correctionFactor = 1.0;
      correctionFactor =  1.001960 * (1.08996 - (1.561100 / std::pow(energy_deposit_/CLHEP::keV, 0.41)));

      // correction is negative bellow 2.5 keV ...
      if (correctionFactor < 0) correctionFactor = 0;
     
      return correctionFactor;
    }
       
  } // end of namespace processing

} // end of namespace snemo
