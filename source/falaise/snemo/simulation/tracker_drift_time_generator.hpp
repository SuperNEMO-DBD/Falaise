/// \file falaise/snemo/simulation/tracker_drift_time_generator.hpp
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2024-07-02
 * Last modified: 2024-07-02
 *
 * Description: Tracker drifttime generator for simulation
 *
 */

#ifndef FALAISE_SNEMO_SIMULATION_TRACKER_DRIFT_TIME_GENERATOR_H
#define FALAISE_SNEMO_SIMULATION_TRACKER_DRIFT_TIME_GENERATOR_H

// Third party:
// - Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/mygsl/rng.h>

// This project:
#include <falaise/snemo/time/time_utils.h>
#include <falaise/snemo/services/service_traits.h>
#include <falaise/snemo/services/service_handle.h>
#include <falaise/snemo/physics_model/tracker_drift_model.hpp>

namespace snemo {

  namespace simulation {

    class tracker_drift_time_generator
    {
    public:
      
      tracker_drift_time_generator();
      
      tracker_drift_time_generator(const datatools::properties & config_,
				   const datatools::service_dict_type & services_);
      
      ~tracker_drift_time_generator() = default;

      void initialize(const datatools::properties & config_,
		      const datatools::service_dict_type & services_);
      
      void reset();

      struct last_ionization_info
      {
	geomtools::vector_3d position;
	geomtools::vector_3d cell_center;
	double drift_time;
	uint32_t nb_ionizations = 0;
      };
      
      double generate_drift_time(const geomtools::geom_id & cell_gid_,
				 const time::time_point & event_reference_timepoint_,
				 const geomtools::vector_3d & position_start_,
				 const geomtools::vector_3d & position_stop_,
				 const double time_start_,
				 const double time_stop_,
				 const double energy_deposit_,
				 mygsl::rng & random_) const;
				 
      const last_ionization_info & last() const;
      
    private:

      double _mean_ionization_energy_ = 50. * CLHEP::eV;
      const geomtools::manager * _geomgr_ = nullptr; ///< Geometry manager
      const snemo::geometry::gg_locator * _gg_locator_ = nullptr; ///< Geometry tracker/Geiger cell locator
      std::unique_ptr<physics_model::tracker_drift_model> _drift_model_ptr_;
      last_ionization_info _last_;
      
    };

  } // end of namespace simulation

} // end of namespace snemo

#endif // FALAISE_SNEMO_SIMULATION_TRACKER_DRIFT_TIME_GENERATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

