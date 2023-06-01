/** \file falaise/snemo/reconstruction/trackfit_driver.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 *                 François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-10-07
 * Last modified : 2022-12-13
 *
 * Copyright (C) 2012-2014 Xavier Garrido <garrido@lal.in2p3.fr>
 * Copyright (C) 2012-2022 François Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   A driver class that wraps the TrackFit tracker fitting algorithm.
 *
 */

#ifndef FALAISE_TRACKFIT_PLUGIN_SNEMO_RECONSTRUCTION_TRACKFIT_DRIVER_H
#define FALAISE_TRACKFIT_PLUGIN_SNEMO_RECONSTRUCTION_TRACKFIT_DRIVER_H 1

// Standard library:
#include <list>
#include <map>
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/clhep_units.h>

// Falaise:
#include <falaise/snemo/processing/base_tracker_fitter.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/datamodels/tracker_cluster.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>
#include <falaise/snemo/datamodels/tracker_trajectory_solution.h>

// This project:
#include <TrackFit/gg_hit.h>
#include <TrackFit/helix_fit_mgr.h>
#include <TrackFit/i_drift_time_calibration.h>
#include <TrackFit/line_fit_mgr.h>

namespace TrackFit {
  struct line_fit_solution;
  struct helix_fit_solution;
  struct line_fit_params;
  struct helix_fit_params;
}  // namespace TrackFit

namespace geomtools {
  class placement;
  class i_shape_1d;
}  // namespace geomtools

namespace snemo {

  namespace processing {
    class geiger_regime;
  }

  namespace reconstruction {

    /// SuperNEMO drift time calibration using snemo::processing::geiger_regime
    class snemo_drift_time_calibration
      : public TrackFit::i_drift_time_calibration
    {
    public:
      /// Constructor
      snemo_drift_time_calibration();

      /// Destructor
      virtual ~snemo_drift_time_calibration();

      virtual double get_sensitive_cell_radius() const;
      virtual double get_max_cell_radius() const;
      virtual void drift_time_to_radius(double time_, double& radius_, double& sigma_radius_) const;
      virtual void radius_to_drift_time(double radius_, double& time_, double& sigma_time_) const;

    private:
      boost::scoped_ptr<snemo::processing::geiger_regime> _gg_regime_;  /// Model of the Geiger regime
    };

    /// Driver for the Bayeux/trackfit fitting algorithms
    class trackfit_driver
      : public ::snemo::processing::base_tracker_fitter
    {
    public:

      enum fit_mode_type {
        HELIX  = 0, ///< Helix fit
        LINEAR = 1  ///< Linear fit
      };

      typedef std::map<std::string, TrackFit::helix_fit_params> helix_guess_dict_type;
      typedef std::map<std::string, TrackFit::line_fit_params> line_guess_dict_type;

      static const std::string & trackfit_id();

      /// Get the name of the drift time/radius calibration object
      const std::string & get_drift_time_calibration_label() const;

      /// Set the name of the drift time/radius calibration object
      void set_drift_time_calibration_label(const std::string & label_);

      /// Set the flag to use the line fit
      void set_use_line_fit(const bool use_line_fit_);

      /// Check the flag to use the line fit
      bool use_line_fit() const;

      /// Set the flag to use the helix fit
      void set_use_helix_fit(const bool use_helix_fit_);

      /// Check the flag to use the helix fit
      bool use_helix_fit() const;

      /// Set a collection of guesses for the line fit
      void set_line_only_guesses(const std::vector<std::string> & only_guesses_);

      /// Set a collection of guesses for the helix fit
      void set_helix_only_guesses(const std::vector<std::string> & only_guesses_);

      /// Perform the helix fit
      void do_helix_fit(const TrackFit::gg_hits_col & gg_hits_,
                        std::list<TrackFit::helix_fit_solution> & solutions_);

      /// Perform the line fit
      void do_line_fit(const TrackFit::gg_hits_col & gg_hits_,
                       std::list<TrackFit::line_fit_solution> & solutions_);

      /// Get the (non-mutable) placement associated to the working referential
      const geomtools::placement & get_working_referential() const;

      /// Get the mutable placement associated to the working referential
      geomtools::placement & grab_working_referential();

      /// Get the non-mutable collection of hits
      const TrackFit::gg_hits_col & get_hits_referential() const;

      /// Get the mutable collection of hits
      TrackFit::gg_hits_col & grab_hits_referential();

      /// Constructor
      trackfit_driver();

      /// Destructor
      virtual ~trackfit_driver();

      /// Initialize the clusterizer through configuration properties
      virtual void initialize(const datatools::properties & setup_);

      /// Reset the clusterizer
      virtual void reset();

    protected:
      /// Main clustering method
      virtual int _process_algo(const snemo::datamodel::tracker_clustering_data & clustering_,
                                snemo::datamodel::tracker_trajectory_data & trajectory_);

    private:
      /// Initialize the drift time/radius calibration driver
      void _install_drift_time_calibration_driver_();

      /// Set default values to class members
      void _set_defaults_();

      /// Compute fit parameters for various 'helix' guess
      void _compute_helix_guesses_(const TrackFit::gg_hits_col & gg_hits_,
                                   helix_guess_dict_type & guesses_,
                                   const size_t max_guess_);

      /// Compute fit parameters for various 'line' guess
      void _compute_line_guesses_(const TrackFit::gg_hits_col & gg_hits_,
                                  line_guess_dict_type & guesses_,
                                  const size_t max_guess_);

      /// Compute 'helix' fit parameters
      void _compute_helix_fit_solutions_(const TrackFit::gg_hits_col & gg_hits_,
                                         const helix_guess_dict_type & guesses_,
                                         std::list<TrackFit::helix_fit_solution> & solutions_);

      /// Compute 'line' fit parameters
      void _compute_line_fit_solutions_(const TrackFit::gg_hits_col & gg_hits_,
                                        const line_guess_dict_type & guesses_,
                                        std::list<TrackFit::line_fit_solution> & solutions_);

      void _process_cluster_(const snemo::datamodel::TrackerClusterHdl & h_cluster_,
                             snemo::datamodel::TrackerTrajectorySolutionHdl & h_trajectory_solution_);
      
    private:
      
      uint32_t _trackfit_flag_ = 0;                ///< Special flags for trackfit algorithm
      std::string _drift_time_calibration_label_;  ///< Drift time calibration driver label
      boost::scoped_ptr<TrackFit::i_drift_time_calibration> _dtc_;  ///< Drift time calibration driver
      double _effective_sigma_r_ = 1.0 * CLHEP::mm; ///< Effective dritf radius uncertainty
      double _vertex_max_distance_xy_ = 1.0 * CLHEP::mm; ///< Tolerance for comparison of fitted trajectory patterns (used to remove duplicated trajectories that have been fitted from different guesses)
      double _vertex_max_distance_z_  = 5.0 * CLHEP::mm; ///< Tolerance for comparison of fitted trajectory patterns (used to remove duplicated trajectories that have been fitted from different guesses)
      double _epsilon_decades_to_best_pvalue_ = 5.e-3;
      double _max_decades_to_best_pvalue_   = 6.0;
  

      // Specific to line fit:
      bool _use_line_fit_ = false;                              ///< Flag to use 'line' fit
      TrackFit::line_fit_mgr::guess_utils _line_guess_driver_;  ///< Guess driver for line fit
      std::map<std::string, int> _line_guess_dict_;             ///< Guess dictionary for 'line' fit
      datatools::properties _line_fit_setup_;                   ///< Setup for the 'line' fit algorithm
      TrackFit::gg_hits_col _gg_hits_referential_;              ///< Geiger hits in the best frame ('line' fit)
      geomtools::placement * _working_referential_ = nullptr;   ///< Working referential ('line' fit)

      // Specific to helix fit:
      bool _use_helix_fit_ = false;                               ///< Use 'helix' fit
      TrackFit::helix_fit_mgr::guess_utils _helix_guess_driver_;  ///< Guess driver for helix fit
      std::map<std::string, int> _helix_guess_dict_;              ///< Guess dictionary for 'helix' fit
      datatools::properties _helix_fit_setup_;  ///< Setup for the 'helix' fit algorithm
      
    };

  }  // end of namespace reconstruction

}  // end of namespace snemo

// Declare the OCD interface of the module
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snemo::reconstruction::trackfit_driver)

#endif  // FALAISE_TRACKFIT_PLUGIN_SNEMO_RECONSTRUCTION_TRACKFIT_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
