// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodel/precalibrated_calorimeter_hit.cc
 */

// Ourselves
#include <falaise/snemo/datamodels/precalibrated_calorimeter_hit.h>

// Third party
// - Bayeux/datatools
#include <datatools/clhep_units.h>
#include <datatools/utils.h>

namespace snemo {

  namespace datamodel {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(precalibrated_calorimeter_hit,
                                                      "snemo::datamodel::precalibrated_calorimeter_hit")

    bool precalibrated_calorimeter_hit::is_valid() const {
      return this->base_hit::is_valid();
    }

    void precalibrated_calorimeter_hit::invalidate() {
      this->base_hit::invalidate();
    }

    void precalibrated_calorimeter_hit::tree_dump(std::ostream& out, const std::string& title,
                                                  const std::string& indent, bool /* is_last */) const {
      base_hit::tree_dump(out, title, indent, true);
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
