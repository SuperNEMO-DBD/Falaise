// -*- mode: c++ ; -*-
/* gg_step_hit_processor.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-07-04
 * Last modified: 2012-10-24
 *
 * License:
 *
 * Description:
 *
 *  A basic processor of simulated step hits in some drift cell
 *  in Geiger regime.
 *
 *  This algorithm randomize first-ionization electrons along the step
 *  and compute the minimal drift distance to the anode wire as well
 *  the corresponding longitudinal position along the wire:
 *  the output step contains the geometry ID of the cell,
 *  the position and time of the fastest ion/electron ionization pair.
 *
 *  CAUTION: this processor can manage only one geometry mapping category
 *  so care should be taken to attach only one geometry model (with the proper
 *  mapping category) to this processor.
 *
 *
 * History:
 *
 */

#ifndef FLSNMC_GG_STEP_HIT_PROCESSOR_H_
#define FLSNMC_GG_STEP_HIT_PROCESSOR_H_ 1

// Third party
// - datatools
#include <datatools/time_tools.h>

// - mygsl (Random generator)
#include <mygsl/rng.h>

// - geomtools (locators)
#include <geomtools/smart_id_locator.h>

// - mctools (interface) :
#include <mctools/base_step_hit_processor.h>

// - Falaise (geometry)
#include <falaise/gg_locator.h>

namespace snemo {

  namespace mc {

    MCTOOLS_STEP_HIT_PROCESSOR_CLASS_DECLARE(gg_step_hit_processor)
    {
    public:

      mygsl::rng & get_rng ();

      bool has_external_rng () const;

      virtual bool accept_external_rng () const;

      virtual void set_external_rng (mygsl::rng & rng_);

      gg_step_hit_processor ();

      virtual ~gg_step_hit_processor ();

      virtual void reset ();

      //! Main setup routine:
      MCTOOLS_STEP_HIT_PROCESSOR_INITIALIZE_DECLARE();

      //! Main processing routine :
      MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_HANDLE_DECLARE();

      //! Main processing routine :
      MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_PLAIN_DECLARE();

      //! Check if a step hit is a candidate for being part within
      // a proposed Geiger hit:
      bool match_gg_hit (const mctools::base_step_hit & gg_hit_,
                         const mctools::base_step_hit & step_hit_) const;

    protected:

      void _set_defaults ();

      void _process (const mctools::base_step_hit_processor::step_hit_ptr_collection_type & base_step_hits_,
                     mctools::simulated_data::hit_handle_collection_type * gg_hits_,
                     mctools::simulated_data::hit_collection_type * plain_gg_hits_);

      void _purge_gg_hits (mctools::simulated_data::hit_handle_collection_type * gg_hits_,
                           mctools::simulated_data::hit_collection_type * plain_gg_hits_);

    private:

      std::string _module_category_; /* the name of the mapping
                                      * category of module
                                      */
      std::string _mapping_category_; /* the name of the mapping
                                       * category of volumes
                                       * that should be considered
                                       * as a sensitive volume
                                       */

      double _time_resolution_; /* the time resolution of the
                                 * Geiger regime (typically 15-25 ns)
                                 */

      double _fiducial_drift_radius_; /* the radius of the effective
                                       * drift region
                                       */

      double _fiducial_drift_length_; /* the active length of the
                                       * drift region
                                       */

      double _mean_ionization_energy_; /* the mean ionization energy
                                        * in the tracking gas:
                                        *
                                        * Default: ~50 eV
                                        *
                                        * From Georges Szkarlz
                                        *   Helium:
                                        *     ~6 e- / cm
                                        *   Helium+Alcohol+Argon:
                                        *     10-15 e- / cm
                                        */

      double _geiger_dead_time_; /* Dead time before a Geiger cell
                                  * is active again after a former hit
                                  *
                                  * From Georges Szkarlz:
                                  *   typically 1-2 ms
                                  *
                                  */
      bool _use_continuous_ionization_;
      bool _compute_minimum_approach_position_;
      bool _store_track_infos_;

      // pseudo-random numbers generator:
      mygsl::rng   _rng_;
      mygsl::rng * _external_rng_;

      // internals:
      const geomtools::mapping * _mapping_; // the reference geometry ID mapping
      const geomtools::id_mgr::categories_by_name_col_type * _categories_;
      uint32_t _module_type_; // the integral ID of the geometry category for module volume
      geomtools::smart_id_locator _module_locator_; /* a locator to compute the
                                                     * module number
                                                     * some hit lies in.
                                                     */
      uint32_t _gg_cell_type_; // the integral ID of the geometry category for gg drift volume
      geomtools::smart_id_locator _gg_cell_locator_; /* a locator to compute the
                                                      * geometry ID of the detector
                                                      * block some hit lies in.
                                                      */
      // Locators:
      geometry::gg_locator _fast_gg_cell_locator_;
      std::map<uint32_t,geometry::gg_locator> _fast_gg_cell_locators_per_module_;

      // Used only for performance test in debug mode:
      datatools::computing_time _CT1_;
      datatools::computing_time _CT2_;

      // Registration macro :
      MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE (gg_step_hit_processor);
    };

  } // end of namespace mc

} // end of namespace snemo

#endif // FLSNMC_GG_STEP_HIT_PROCESSOR_H_

// end of gg_step_hit_processor.h
