// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/calibrated_calorimeter_hit.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-16
 * Last modified: 2014-01-27
 *
 * License:
 *
 * Description:
 *   Calibrated calorimeter hit
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_CALIBRATED_CALORIMETER_HIT_H
#define FALAISE_SNEMO_DATAMODELS_CALIBRATED_CALORIMETER_HIT_H 1

// Third party
// - Bayeux/datatools
#include <datatools/handle.h>
// - Bayeux/geomtools
#include <geomtools/base_hit.h>

namespace snemo {

    namespace datamodel {

      /// \brief Model of a calibrated calorimeter hit
      class calibrated_calorimeter_hit : public geomtools::base_hit
      {
      public:

        /// Handle of calibrated calorimeter hit
        typedef datatools::handle<calibrated_calorimeter_hit> handle_type;

        /// Collection of handles of calibrated calorimeter hit
        typedef std::vector<handle_type>                      collection_type;

        /// Return the time associated to the hit
        double get_time() const;

        /// Set the time associated to the hit
        void set_time(double);

        /// Return the error on the time associated to the hit
        double get_sigma_time() const;

        /// Set the error on the time associated to the hit
        void set_sigma_time(double);

        /// Return the energy associated to the hit
        double get_energy() const;

        /// Set the energy associated to the hit
        void set_energy(double);

        /// Return the error on the energy associated to the hit
        double get_sigma_energy() const;

        /// Set the error on the energy associated to the hit
        void set_sigma_energy(double);

        /// Constructor
        calibrated_calorimeter_hit();

        /// Destructor
        virtual ~calibrated_calorimeter_hit();

        /// Check if the internal data of the hit are valid
        bool is_valid() const;

        /// Invalidate the internal data of hit
        void invalidate();

        /// Smart print
        virtual void tree_dump(std::ostream      & a_out    = std::clog,
                               const std::string & a_title  = "",
                               const std::string & a_indent = "",
                               bool a_inherit               = false) const;

        /// Basic print
        void dump() const;

      private:

        double _energy_;       //!< Energy associated to the hit
        double _sigma_energy_; //!< Error on the energy associated to the hit
        double _time_;         //!< Time associated to the hit
        double _sigma_time_;   //!< Error on the time associated to the hit

        DATATOOLS_SERIALIZATION_DECLARATION();

      };

    } // end of namespace datamodel

} // end of namespace snemo

//#include <boost/serialization/export.hpp>
//BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::calibrated_calorimeter_hit, "snemo::datamodel::calibrated_calorimeter_hit")

#endif // FALAISE_SNEMO_DATAMODELS_CALIBRATED_CALORIMETER_HIT_H

// end of falaise/snemo/datamodels/calibrated_calorimeter_hit.h
