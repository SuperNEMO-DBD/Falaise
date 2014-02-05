// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/gg_track_utils.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-07-04
 * Last modified: 2014-01-30
 */

#ifndef FALAISE_SNEMO_DATAMODEL_GG_TRACK_UTILS_H
#define FALAISE_SNEMO_DATAMODEL_GG_TRACK_UTILS_H 1

// Standard library
#include <string>

namespace snemo {

  namespace datamodel {

    struct gg_track {

      /// The name of a real property representing the minimal approach distance of the particle track to the anode wire
      static const std::string & minimum_approach_distance_key();

      /// The name of a vector of real (x,y,z) triplet property representing the position of the particle track at minimal approach distance to the anode wire
      static const std::string & minimum_approach_position_key();

      /// The name of a property flag that tags some missing Geiger hit along the particle track
      static const std::string & missing_geiger_hit_flag();

    };

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODEL_GG_TRACK_UTILS_H
