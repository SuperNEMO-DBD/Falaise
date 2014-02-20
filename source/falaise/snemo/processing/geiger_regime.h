// -*- mode: c++ ; -*-
/* * \file falaise/snemo/processing/geiger_regime.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-01-09
 * Last modified: 2014-01-30
 *
 * License:
 *
 * Description:
 *
 *   Tools to model the Geiger regime in SuperNEMO drift cells.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_GEIGER_REGIME_H
#define FALAISE_SNEMO_PROCESSING_GEIGER_REGIME_H 1

// Standard library:
#include <string>
#include <iostream>

// Third party:
// - Bayeux/datatools
#include <datatools/i_tree_dump.h>
// - Bayeux/mygsl:
#include <mygsl/tabulated_function.h>

namespace datatools {
  class properties;
}

namespace mygsl {
  class rng;
}

namespace snemo {

    namespace processing {

      class geiger_regime : public datatools::i_tree_dumpable
      {
      public:

        double get_cell_diameter () const;

        double get_cell_radius () const;

        double get_cell_length () const;

        double get_sigma_anode_time (double anode_time_) const;

        double get_sigma_cathode_time () const;

        double get_t0 () const;

        double get_tcut () const;

        double get_r0 () const;

        double get_rdiag () const;

        double get_base_anode_efficiency () const;

        double get_base_cathode_efficiency () const;

        double get_anode_efficiency (double r_) const;

        double get_cathode_efficiency () const;

        double get_plasma_longitudinal_speed () const;

        double get_sigma_plasma_longitudinal_speed () const;

        mygsl::tabulated_function & grab_base_rt ();

        /// Check initialization flag
        bool is_initialized () const;

        /// Default constructor
        geiger_regime ();
        
        virtual ~geiger_regime() {;}

        /// Initialization from parameters
        void initialize (const datatools::properties & config_);

        /// Reset
        void reset ();

        double randomize_z (mygsl::rng & ran_, double z_, double sigma_z_) const;

        double randomize_r (mygsl::rng & ran_, double r_) const;

        double randomize_drift_time_from_drift_distance (mygsl::rng & ran_,
                                                         double drift_distance_) const;

        double base_t_2_r (double time_, int mode_ = 0) const;

        double get_sigma_z (double z_, size_t missing_cathodes_ = 0) const;

        double get_sigma_r (double r_) const;

        void calibrate_drift_radius_from_drift_time (double   drift_time_,
                                                     double & drift_radius_,
                                                     double & sigma_drift_radius_) const;

         /// Smart print
        virtual void tree_dump (std::ostream & a_out         = std::clog,
                                const std::string & a_title  = "",
                                const std::string & a_indent = "",
                                bool a_inherit               = false) const;

      private:

        bool _initialized_; //!< Initalization flag

        double _cell_diameter_; //!< Fiducial drift diameter of a cell
        double _cell_length_;   //!< Fiducial drift length of a cell
        double _sigma_anode_time_;   //!< Anode TDC resolution
        double _sigma_cathode_time_; //!< Cathode TDC resolution
        double _sigma_z_;                 //!< Longitudinal resolution
        double _sigma_z_missing_cathode_; //!< Longitudinal resolution for one missing cathode

        // parameters for transverse resolution:
        double _sigma_r_a_;  //!< Parameters 0 for transverse resolution
        double _sigma_r_b_;  //!< Parameters 1 for transverse resolution
        double _sigma_r_r0_; //!< Parameters 2 for transverse resolution

        double _base_anode_efficiency_; //!< Base anode efficiency
        double _base_cathode_efficiency_; //!< Base cathode efficiency
        double _plasma_longitudinal_speed_; //!< Plasma longitudinal speed
        double _sigma_plasma_longitudinal_speed_; //!< Error on plasma longitudinal speed

        // internals:
        mygsl::tabulated_function _base_rt_; //!< Tabluated function for drift time<->drift radius association
        double _t0_;    //!< Time (not documented yet)
        double _r0_;    //!< Radius (not documented yet)
        double _rdiag_; //!< Cut on drift radius (not documented yet)
        double _tcut_;  //!< Cut on drift time (not documented yet)

      private:

        /// Set default values for attributes
        void _init_defaults_ ();

      };

    } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_GEIGER_REGIME_H

// end of falaise/snemo/processing/geiger_regime.h
