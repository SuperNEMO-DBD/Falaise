// -*- mode: c++ ; -*-
/** \file falaise/TrackFit/line_fit_mgr.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                Arnaud Chapon   <chapon@lpccaen.in2p3.fr>
 * Creation date: 2010-02-15
 * Last modified: 2014-02-08
 *
 * Copyright 2012-2014 F. Mauger
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
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *   Fit a line from a collection of Geiger hits
 *
 * History:
 *
 */

#ifndef FALAISE_TRACKFIT_LINE_FIT_MGR_H
#define FALAISE_TRACKFIT_LINE_FIT_MGR_H 1

// Standard library:
#include <sstream>

// Third party:
// - Boost:
#include <boost/utility.hpp>
// - GSL:
#include <gsl/gsl_multifit_nlin.h>

// This project:
#include <TrackFit/gg_hit.h>

namespace geomtools {
  class placement;
  class line_3d;
}

namespace TrackFit {

  /// Drift time to radius calibration interface
  struct i_drift_time_calibration;

  /// \brief Parameters of the line fit
  struct line_fit_params
  {
    /// Number of parameters of the line fit
    static const size_t LINE_FIT_NOPARS = 5;

    /// \brief Index associated to each parameter of the line fit
    enum param_index_type {
        PARAM_INDEX_INVALID = -1,
        PARAM_INDEX_Z0      =  0, /// Index of the Z0 free parameter
        PARAM_INDEX_Y0      =  1, /// Index of the Y0 free parameter
        PARAM_INDEX_PHI     =  2, /// Index of the PHI free parameter
        PARAM_INDEX_THETA   =  3, /// Index of the THETA free parameter
        PARAM_INDEX_T0      =  4, /// Index of the T0 free parameter (if used)
      };

    /// Constructor
    line_fit_params();

    /// Check the validity of the data
    bool is_valid() const;

    /// Reset
    void reset();

    /// Smart print
    void draw(std::ostream & out_, double length_) const;

    // Attributes:
    double y0;    /// Y coordinate of the reference point of the fitted line
    double z0;    /// Z coordinate of the reference point of the fitted line
    double phi;   /// Phi angle of the direction of the fitted line
    double theta; /// Theta angle of the direction of the fitted line
    double t0;    /// Reference time (default = 0, set by user)

  };


  /// \brief Input data for the line fit
  struct line_fit_data
  {
    /// Default constructor
    line_fit_data();
    /// Check validity
    bool is_valid() const;
    /// Reset
    void reset();

    // Attributes:
    bool using_first;         /// Use first flag (default = false)
    bool using_last;          /// Use last flag (default = false)
    bool using_drift_time;    /// Use drift time (default = false)
    bool fit_start_time;      /// Flag to also fit the reference time
    const gg_hits_col * hits; /// Collection of Geiger hits
    const i_drift_time_calibration * calibration; /// Handle to the drift time to radius calibration object
  };

  /// \brief The solution of the line fit
  struct line_fit_solution : public line_fit_params
  {
   /// Constructor
    line_fit_solution();
   /// Destructor
    ~line_fit_solution();
    /// Reset
    void reset();
   /// Compute P-probability
    double probability_p() const;
    /// Compute Q-probability
    double probability_q() const;
    /// Basic print of the solution
    void tree_dump(std::ostream      & out_ = std::clog,
                   const std::string & title_ = "",
                   const std::string & indent_ = "",
                   bool inherit_ = false) const;
    // Attributes:
    bool   ok;        /// Status if the solution
    double err_t0;    /// Error on the T0 (if used)
    double err_y0;    /// Error on the Y0
    double err_z0;    /// Error on the Z0
    double err_phi;   /// Error on phi
    double err_theta; /// Error on theta
    double chi;       /// Chi value
    size_t ndof;      /// Number of degrees of freedom
    size_t niter;     /// Number of iterations of the fit
    datatools::properties auxiliaries; /// Auxiliary properties
  };

  /// \brief Parameters of the residual function
  struct line_fit_residual_function_param
  {
    /// \brief Type of residuals
    enum residual_type {
      RESIDUAL_INVALID = -1, /// Invalid value
      RESIDUAL_ALPHA   =  0, /// Identifier of the 'alpha' residuals (computed in the XY Geiger drift plane)
      RESIDUAL_BETA    =  1  /// Identifier of the 'beta' residuals (computed along the Z Geiger plasma propagation axis)
    };

    /// Default constructor
    line_fit_residual_function_param();

    // Attributes:
    bool   using_first;      /// Use the first flag
    bool   using_last;       /// Use the last flag
    bool   using_drift_time; /// Use the drift time
    bool   fit_start_time;   /// Fit the start time
    int    mode;             /// Mode
    int    residual_type;    /// Type of residual
    bool   first;            /// First flag
    bool   last;             /// Last flag
    double xi, yi, zi, szi, ti, ti_min, ri, dri, rmaxi; /// Data points
    double t0;                  /// Reference time (eventually a free paramter)
    double y0, z0, phi, theta;  /// Free parameters
    const i_drift_time_calibration * dtc; /// Handle to the drift time to radius calibration
  };

  /// \brief Manager of the line fit
  class line_fit_mgr : boost::noncopyable
  {
  public:

    /// \brief Constants
    struct constants {
      static unsigned int default_fit_max_iter(); /// Default maximum number of iterations of the fit
      static double       default_fit_eps();      /// Default tolerance of the fit
      static unsigned int min_number_of_hits();   /// Minimum number of hits to perform the fit
    };

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority priority_);

    /// Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Check the initialization flag
    bool is_initialized() const;

    /// Check the debug flag
    bool is_debug() const;

    /// Set the debug flag
    void set_debug(bool);

    /// Check if the fit uses the 'last' flag of hits
    bool is_using_last() const;

    /// Check if the fit uses the 'first' flag of hits
    bool is_using_first() const;

    /// Check if the fit uses the drift time
    bool is_using_drift_time() const;

    /// Check if the fit fits the reference time
    bool is_fitting_start_time() const;

    /// Set the fit tolerance
    void set_fit_eps(double eps_);

    /// Set the reference time of the hits(if not part of the free parameters)
    void set_t0(double);

    /// Return the non-mutable solution of the fit
    const line_fit_solution & get_solution();

    /// Return the mutable solution of the fit
     line_fit_solution & grab_solution();

    /// Set the fit guess
    void set_guess(const line_fit_params &);

    /// Set the collection of hits to be fitted
    void set_hits(const gg_hits_col & hits_);

    /// Set the calibration object to be used along the fit
    void set_calibration(const i_drift_time_calibration & calibration_);

    /// Check if a calibration object is available
    bool has_calibration() const;

    /// Default constructor
    line_fit_mgr(bool debug_ = false);

    /// Destructor
    virtual ~line_fit_mgr();

    /// Initialization from parameters
    void initialize(const datatools::properties & config_);

    /// Initialization from parameters
    void init(const datatools::properties & config_);

    /// Reset
    void reset();

    /// Print the fit status
    void print_fit_status(std::ostream & out_ = std::clog) const;

    /// Action perfomed at each step
    virtual void at_fit_step_do();

    /// Perform the fit
    void fit();

    /// \brief Utility class for building input fit guess
    struct guess_utils
    {
    public:

      /// Number of possible guess
      static const size_t NUMBER_OF_GUESS = 4;

      /// \brief Guess mode type
      /**
       *   4 different guess lines can be built from two
       *   Geiger hits with enough distance between them.
       *   In the working reference frame (O'X'Y'Z'):
       *    - [T] corresponds to a point just above the starting or stopping cell
       *    - [B] corresponds to a point just below the starting or stopping cell
       *   Thus, we can build the following candidate segment to initiate
       *   the fit: (TT), (TB), (BT), (BB).
       *
       *                         ^ Y'
       *                         |
       *                         |         [T]
       *             [T]         |          :
       *              :        O'|       o  o <-- stopping
       *         - - -o- o - - - + - -o -o- :- - - - - -> X'
       * starting --> :  o  o  o | o       [B]
       *             [B]         |
       *                         |
       *                         |
       *
       */
      enum guess_mode_type {
        GUESS_MODE_BB = 0, // Bottom-Bottom guess
        GUESS_MODE_BT = 1, // Bottom-Top guess
        GUESS_MODE_TB = 2, // Top-Bottom guess
        GUESS_MODE_TT = 3  // Top-Top guess
      };

      /// Return the label from a guess mode
      static std::string guess_mode_label(int);

      /// Default constructor
      guess_utils();

      /// Initialization from parameters
      void initialize(const datatools::properties & config_);

      /// Reset
     void reset();

      /// Compute a initial guess for the fit
      bool compute_guess(const gg_hits_col & hits_,
                          int guess_mode_,
                          line_fit_params & guess_);

    protected :

     /// Set default attribute values
       void _set_defaults();

    private :

      datatools::logger::priority _logging_priority_; /// Logging priority threshold
      bool   _use_max_radius_;       /// Flag to use the maximum radius
      double _max_radius_factor_;    /// Factor for maximum radius
      bool   _use_guess_trust_;      /// Flag to use guess trust
      int    _guess_trust_mode_;     /// Mode for guess trust
      bool   _fit_delayed_clusters_; /// Flag to fit delayed clusters

    };

    /// Compute the best reference frame
    static void compute_best_frame(const gg_hits_col & hits_,
                                    gg_hits_col & hits_ref_,
                                    geomtools::placement & pl_,
                                    const uint32_t flags_ = 0);
    /*
    static bool compute_guess(const gg_hits_col & hits_,
                               line_fit_params & guess_,
                               int guess_mode_,
                               datatools::properties & config_,
                               const uint32_t flags_ = 0);
    */

    /// Compute residual(GSL interface)
    static double residual_function(double x_, void * params_);

    /// Compute residual(GSL interface)
    static int residual_f(const gsl_vector * x_,
                           void *             params_,
                           gsl_vector *       f_);

    /// Compute residual difference(GSL interface)
    static int residual_df(const gsl_vector * x_,
                            void *             params_,
                            gsl_matrix *       J_);

    /// Compute residual and difference(GSL interface)
    static int residual_fdf(const gsl_vector * x_,
                             void *             params_,
                             gsl_vector *       f_,
                             gsl_matrix *       J_);

    /// Access to residual parameters associated to an individual hit
    void get_residuals_per_hit(size_t hit_index_,
                                double & alpha_residual_,
                                double & beta_residual_,
                                bool at_solution_ = false) const;

    /// Generate display data for the temporary solution
    void draw_temporary_solution(std::ostream & out_) const;

    /// Generation of display data for solution
    void draw_solution(std::ostream & out_, const line_fit_solution &) const;

    /// Generation of display data for embedded solution
    void draw_solution(std::ostream & out_) const;

    /// Convert solution in the user reference frame
    static void convert_solution(const gg_hits_col & hits_ref_,
                                  const line_fit_solution & sol_,
                                  const geomtools::placement & pl_,
                                  geomtools::line_3d & line_);

  protected:

    /// Set the initialization flag
    void _set_initialized(bool);

  private:

    /// Set default attribute values
    void _set_defaults_();

  private:

    datatools::logger::priority _logging_priority_; /// Logging priority threshold

    bool _initialized_; /// Initialization flag

    // GSL multifit stuff:
    size_t                    _fit_npoints_;         /// Number of points
    size_t                    _fit_npars_;           /// Number of fitted parameters
    gsl_multifit_fdfsolver *  _fit_mf_fdf_solver_;   /// GSL solver
    gsl_multifit_function_fdf _fit_mf_fdf_function_; /// GSL fit function
    gsl_vector_view           _fit_vview_;           /// GSL view for the internal vector of fit parameters
    double                    _fit_x_init_[line_fit_params::LINE_FIT_NOPARS]; /// Parameters for the fitted line
    size_t                    _fit_iter_;     /// Current number of fit iterations
    double                    _fit_eps_;      /// Fit tolerance
    size_t                    _fit_max_iter_; /// Maximum number of fit iterations
    gsl_matrix *              _fit_covar_;    /// Covariance matrix of the fit
    int                       _fit_status_;   /// Current fit status
    line_fit_data             _fit_data_;     /// Fit data for a line

    bool _using_last_;       /// Flag to use the 'last' flag of hits
    bool _using_first_;      /// Flag to use the 'first' flag of hits
    bool _using_drift_time_; /** Flag to use the 'drift time' of hits combined to a calibration object
                              * in place of the pre-calibrarion drift radius. This mode uses a
                              * on-the-fly time-to-radius calibration.
                              */
    bool _fit_start_time_; /// Flag to also consider the reference time as a free parameter
    const gg_hits_col *              _hits_;        /// Handle to the input collection of Geiger hits
    const i_drift_time_calibration * _calibration_; /// Handle to the calibration object
    double _t0_;                  /// Reference delay time (==0 set by user)
    bool _step_print_status_;     /// Flag to print the status of the fit at each step
    bool _step_draw_;             /// Flag to display the fit status at each step
    line_fit_solution _solution_; /// Embedded solution of the fit

  };

} // end of namespace TrackFit

#endif // FALAISE_TRACKFIT_LINE_FIT_MGR_H
